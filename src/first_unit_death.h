#ifndef FIRST_UNIT_DEATH_H
#define FIRST_UNIT_DEATH_H

#include "sc2api/sc2_api.h"

#include "sc2utils/sc2_manage_process.h"

#include <iostream>
#include <fstream>


const int STEP_SIZE = 22;
const int PLAYER_ID = 1;
static int replay_count = 0;


class FirstUnitDeath : public sc2::ReplayObserver {
public:
    int step_num = 0;
    std::vector<std::string> fout_strings;
    std::ofstream fout;
    const char* kReplayFolder_;
    bool halt_data = false;
    int probes = 0; // initial scout

    FirstUnitDeath(const char* ReplayFolder) :
        sc2::ReplayObserver() {
        kReplayFolder_ = ReplayFolder;
    }

    void OpenFile(std::string headers)
    {
        std::string output_path = replay_observer_file_path_.substr(0,replay_observer_file_path_.size() - 10) + "_1st_death.csv";
        fout.open(output_path.c_str(), std::ofstream::out);
        fout_strings.push_back(headers);
    }

    void CloseFile()
    {
        for (std::string x : fout_strings)
            fout << x;
        fout.close();
    }


    // Inherited methods
    
    void OnGameStart() 
    {
        std::cout << "Replay: " << replay_count++ << std::endl;
        step_num = 0;
        halt_data = false;
        fout_strings.clear();
        OpenFile("Seconds,Item,Upgrade\n");
    }
    
    void OnUnitCreated(const sc2::Unit* unit) 
    {
        if (!halt_data && unit->owner == PLAYER_ID)   // Units are "created" when seen, so it "creates" minerals, neutral map mobs, etc.
            if (unit->unit_type != 801) // Lets ignore phase shifting for strategizer?
                fout_strings.emplace_back(std::to_string(GetGameSecond(step_num)) + "," + std::to_string(unit->unit_type) + ",0\n");
    }

    void OnUpgradeCompleted(sc2::UpgradeID upgrade) 
    {
        if (!halt_data)
            fout_strings.emplace_back(std::to_string(GetGameSecond(step_num)) + "," + std::to_string(upgrade) + ",1\n");
    }

    void OnUnitDestroyed(const sc2::Unit* unit)
    {
        if (!halt_data && unit->owner == PLAYER_ID)   // Units are "created" when seen, so it "creates" minerals, neutral map mobs, etc.
        {
            if (unit->unit_type == 84)
            {
                if (probes == 0)
                    ++probes;
                else
                    halt_data = true;
            }
            else if (unit->unit_type != 801 && unit->unit_type != 311) // Lets ignore phase shifting for strategizer?
            {
                halt_data = true;
            }

        }
    }


    void OnStep() 
    {
        step_num += STEP_SIZE;
    }

    void OnGameEnd() 
    {
        CloseFile();
    }
};

#endif