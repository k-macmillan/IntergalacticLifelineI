/*
Gather data necessary for multidemensional dynamic time warping (DTW).
*/

#ifndef DYNAMIC_TIME_WARPING_H
#define DYNAMIC_TIME_WARPING_H

#include "sc2api/sc2_api.h"
#include "sc2api/sc2_score.h"
#include "convex_hull_slow.h"

#include "sc2utils/sc2_manage_process.h"

#include <iostream>
#include <fstream>


const int STEP_SIZE = 22;
const int PLAYER_ID = 1;
static int replay_count = 0;


class DynamicTimeWarping : public sc2::ReplayObserver {
public:
	int step_num = 0;
	std::vector<std::string> fout_strings;
	std::ofstream fout;
	bool halt_data = false;
	int probes = 0, adepts = 0; // initial scout
	const char* kReplayFolder_;

	DynamicTimeWarping(const char* ReplayFolder) :
		sc2::ReplayObserver() {
		kReplayFolder_ = ReplayFolder;
	}

	void OpenFile(std::string headers)
	{
		// std::string output_path = replay_observer_file_path_.substr(0,replay_observer_file_path_.size() - 10) + "_dtw.csv";
		// fout.open(output_path.c_str(), std::ofstream::out);
		// fout_strings.push_back(headers);
	}

	void CloseFile()
	{
		// for (std::string x : fout_strings)
		// fout << x;
		// fout.close();
	}


	// Inherited methods

	void OnGameStart()
	{
		std::cout << "Replay: " << replay_count++ << std::endl;
		step_num = 0;
		probes = 0;
		adepts = 0;
		halt_data = false;
		// fout_strings.clear();
		// OpenFile("Seconds,Item,Upgrade\n");
	}

	void OnUnitCreated(const sc2::Unit* unit)
	{
		// if (unit->owner == PLAYER_ID)   // Units are "created" when seen, so it "creates" minerals, neutral map mobs, etc.
		// if (unit->unit_type != 801) // Lets ignore phase shifting for strategizer?
		// fout_strings.emplace_back(std::to_string(GetGameSecond(step_num)) + "," + std::to_string(unit->unit_type) + ",0\n");
	}

	void OnUpgradeCompleted(sc2::UpgradeID upgrade)
	{
		// fout_strings.emplace_back(std::to_string(GetGameSecond(step_num)) + "," + std::to_string(upgrade) + ",1\n");
	}


	void OnStep()
	{

		// sc2::Units structures = Observation()->GetUnits(sc2::Unit::Self, IsStructure(Observation()));

		if (!halt_data && step_num < 7400)  // Stop at roughly 177 seconds or first unit death
		{
			const sc2::ObservationInterface* obs = Observation();
			const sc2::Score& score = obs->GetScore();
			// sc2::Score score;
			float min_rate = score.score_details.collection_rate_minerals;
			float gas_rate = score.score_details.collection_rate_vespene;
			float upg_min = score.score_details.used_minerals.upgrade;
			float upg_vesp = score.score_details.used_vespene.upgrade;
			float struct_val = score.score_details.total_value_structures;
			sc2::Units units = Observation()->GetUnits();
			float struct_area = convhull::Area(convhull::ConvexHull(GetStructures(units)));

			std::cout << std::to_string(GetGameSecond(step_num)) << "\tarmy value: " << GetArmyValue(obs, units)
				<< "," << min_rate << "," << gas_rate << "," << GetStructuresValue(obs, units) << "," << upg_min << "," << upg_vesp
				<< ",\t" << struct_area << std::endl;
		}
		step_num += STEP_SIZE;
	}

	void OnUnitDestroyed(const sc2::Unit* unit)
	{
		if (!halt_data && unit->owner == PLAYER_ID)   // Units are "created" when seen, so it "creates" minerals, neutral map mobs, etc.
		{
			// std::cout << unit->owner << "\t" << unit->unit_type << std::endl;
			if (unit->unit_type == 84)
			{
				if (probes < 7) // Lets ignore worker-line harass
					++probes;
				// else
				// halt_data = true;
			}
			else if (unit->unit_type == 311)
			{
				if (adepts < 3) // Lets ignore scouts dying
					++adepts;
				// else
				// halt_data = true;
			}
			// else if (DesiredArmyUnit(unit)) // Lets ignore phase shifting for strategizer?
			// {
			// halt_data = true;
			// }
		}

		if (!halt_data && IsStructure(*unit))
			std::cout << unit->unit_type << std::endl;
	}

	void OnGameEnd()
	{
		// CloseFile();
	}

	unsigned int GetArmyValue(const sc2::ObservationInterface* obs, sc2::Units units)
	{
		// This may be built into the scoreboard if I can figure that out...
		unsigned int army_value = 0;
		const sc2::UnitTypes& unit_types = obs->GetUnitTypeData();
		// sc2::Units units = obs->GetUnits();
		for (auto u : units)
		{
			if (DesiredArmyUnit(u))
			{
				army_value += unit_types.at(u->unit_type).mineral_cost;
				army_value += unit_types.at(u->unit_type).vespene_cost;
				// std::cout << std::to_string(u->unit_type) + ",";
			}
		}
		// std::cout << std::endl;
		return army_value;
	}

	unsigned int GetStructuresValue(const sc2::ObservationInterface* obs, sc2::Units units)
	{
		// This may be built into the scoreboard if I can figure that out...
		unsigned int struct_value = 0;
		const sc2::UnitTypes& unit_types = obs->GetUnitTypeData();
		// sc2::Units units = obs->GetUnits();
		for (const sc2::Unit *u : units)
		{
			if (IsStructure(*u))
			{
				struct_value += unit_types.at(u->unit_type).mineral_cost;
				struct_value += unit_types.at(u->unit_type).vespene_cost;
				// std::cout << std::to_string(u->unit_type) + ",";
			}
		}
		// std::cout << std::endl;
		return struct_value;
	}


	bool DesiredArmyUnit(const sc2::Unit*& u)
	{
		// Fix this. Utilize enum class UNIT_TYPEID
		return u->owner == PLAYER_ID && u->is_alive && u->unit_type != 801 && u->unit_type != 341 && u->unit_type != 665 && u->unit_type != 666
			&& u->unit_type != 146 && u->unit_type != 147
			&& u->unit_type != 483 && u->unit_type != 342 && u->unit_type != 608
			&& u->unit_type != 343 && u->unit_type != 84
			&& u->unit_type != 60 && u->unit_type != 61 && u->unit_type != 59 && u->unit_type != 72
			&& u->unit_type != 62 && u->unit_type != 67;
	}

	// // I don't trust this filter function
	// struct IsStructure {
	//     IsStructure(const sc2::ObservationInterface* obs) : observation_(obs) {};

	//     bool operator()(const sc2::Unit& unit) {
	//         if ( !unit.is_alive )
	//             return false;
	//         auto& attributes = observation_->GetUnitTypeData().at(unit.unit_type).attributes;
	//         bool is_structure = false;
	//         for (const auto& attribute : attributes) {
	//             if (attribute == sc2::Attribute::Structure) {
	//                 is_structure = true;
	//             }
	//         }
	//         return is_structure;
	//     }

	//     const sc2::ObservationInterface* observation_;
	// };


	std::vector<const sc2::Unit*> GetStructures(sc2::Units units)
	{
		std::vector<const sc2::Unit*> ret_val;
		ret_val.reserve(units.size());
		for (const sc2::Unit *u : units)
			if (IsStructure(*u))
				ret_val.push_back(u);
		return ret_val;
	}

	bool IsStructure(const sc2::Unit &unit)
	{
		return (unit.owner == PLAYER_ID && unit.is_alive && (unit.unit_type == 59 || unit.unit_type == 60 || unit.unit_type == 62 || unit.unit_type == 63 || unit.unit_type == 66 || unit.unit_type == 133));

	}

};

#endif