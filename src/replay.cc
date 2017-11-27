#include "sc2api/sc2_api.h"

#include "sc2utils/sc2_manage_process.h"
#include "time_unit_created.h"
#include "sc2api/config.h"

#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char* argv[]) {
    Config bot_config(replay_config_file);

    sc2::Coordinator coordinator;
    if (!coordinator.LoadSettings(argc, argv, bot_config.exe_path)) {
        return 1;
    }


    TimeUnitCreated replay_observer;

    if (!coordinator.SetReplayPath(bot_config.replay_path.c_str())) {
        std::cout << "Unable to find replays." << std::endl;
        return 1;
    }

    coordinator.SetStepSize(STEP_SIZE);
    coordinator.AddReplayObserver(&replay_observer);

    while (coordinator.Update());
    while (!sc2::PollKeyPress());
}