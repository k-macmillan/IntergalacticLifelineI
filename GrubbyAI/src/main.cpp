#include "Grubby.h"
#include "Maps.h"




int main(int argc, char* argv[]) {
	sc2::Coordinator coordinator;
	if (!coordinator.LoadSettings(argc, argv)) {
		return 1;
	}

	// Add the custom bot, it will control the players.
	Grubby bot;
	coordinator.SetMultithreaded(true);

	coordinator.SetParticipants({
		CreateParticipant(sc2::Race::Zerg, &bot),
		CreateComputer(sc2::Race::Random, sc2::Difficulty::Easy)
	});


	// Start the game.
	coordinator.LaunchStarcraft();

	// Step forward the game simulation.
	bool do_break = false;

	while (!do_break) {
		// Select random map from available maps
		coordinator.StartGame(sc2maps[sc2::GetRandomInteger(0, int(sc2maps.size()) - 1)]);
		while (coordinator.Update() && !do_break) {
		}
	}

	return 0;
}

