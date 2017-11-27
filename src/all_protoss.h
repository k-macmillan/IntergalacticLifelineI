#ifndef ALL_PROTOSS_H
#define ALL_PROTOSS_H

#include "sc2api/sc2_api.h"

#include "sc2utils/sc2_manage_process.h"

#include <iostream>
#include <fstream>

const int STEP_SIZE = 22;
const int PLAYER_ID = 1;
static int replay_count = 0;


class AllProtoss : public sc2::ReplayObserver {
public:
    int step_num = 0;
    std::vector<std::string> fout_strings;
    std::ofstream fout;
    const char* kReplayFolder_;

    AllProtoss(const char* ReplayFolder) :
        sc2::ReplayObserver() {
        kReplayFolder_ = ReplayFolder;
    }

    void OpenFile(std::string headers)
    {
        std::string output_path = replay_observer_file_path_.substr(0,replay_observer_file_path_.size() - 10) + "all_protoss.csv";
        fout.open(output_path.c_str(), std::ofstream::out);
        fout_strings.push_back(headers);
    }

    void CloseFile()
    {
        for (std::string x : fout_strings)
            fout << x;
        fout.close();
    }

    void PrintAllProtoss()
    {
        const sc2::ObservationInterface* obs = Observation();
        sc2::Units units = obs->GetUnits();

        int probe_count = 0;
        int zealot_count = 0;
        int stalker_count = 0;
        int adept_count = 0;
        int gateway_count = 0;
        int warpgate_count = 0;
        int pylon_count = 0;
        int forge_count = 0;
        int nexus_count = 0;
        int cyber_count = 0;
        int cannon_count = 0;
        int assim_count = 0;


        for (int i = 0; i < units.size(); ++i)
        {
            if (units[i]->owner == PLAYER_ID)
            {
                // Units
                if (units[i]->unit_type == 84)
                    ++probe_count;
                if (units[i]->unit_type == 73)
                    ++zealot_count;
                if (units[i]->unit_type == 74)
                    ++stalker_count;
                if (units[i]->unit_type == 133)
                    ++adept_count;

                // Structures
                if (units[i]->unit_type == 62)
                    ++gateway_count;
                if (units[i]->unit_type == 133)
                    ++warpgate_count;
                if (units[i]->unit_type == 60)
                    ++pylon_count;
                if (units[i]->unit_type == 63)
                    ++forge_count;
                if (units[i]->unit_type == 59)
                    ++nexus_count;
                if (units[i]->unit_type == 72)
                    ++cyber_count;
                if (units[i]->unit_type == 66)
                    ++cannon_count;

                // Resource
                if (units[i]->unit_type == 61)
                    ++assim_count;
            }
        }
        std::string all_protoss_units = std::to_string(step_num) + "," + std::to_string(probe_count) + "," + std::to_string(zealot_count) + "," + std::to_string(stalker_count) + "," + 
                                        std::to_string(adept_count) + "," + std::to_string(gateway_count) + "," + std::to_string(warpgate_count) + "," + std::to_string(pylon_count) + "," + 
                                        std::to_string(forge_count) + "," + std::to_string(nexus_count) + "," + std::to_string(cyber_count) + "," + std::to_string(assim_count) + "," + 
                                        std::to_string(cannon_count) + "\n";
        fout_strings.push_back(all_protoss_units);
    }


    // Inherited methods

    void OnGameStart() 
    {
        std::cout << "Replay: " << replay_count++ << std::endl;

        step_num = 0;
        fout_strings.clear();
        OpenFile("Step,Probe,Zealot,Stalker,Adept,Gateway,Warpgate,Pylon,Forge,Nexus,Cybercore,Assimilator,Cannon\n");
    }

    void OnStep() 
    {
        PrintAllProtoss();
        step_num += STEP_SIZE;
    }

    void OnGameEnd() 
    {
        CloseFile();
    }
};

#endif