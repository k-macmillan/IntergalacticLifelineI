#ifndef ADDITIONAL_FUNCTIONALITY_H
#define ADDITIONAL_FUNCTIONALITY_H

#include "sc2api/sc2_api.h"

/////////////////////////////////////////////////////////////////////////////
////////////////////////////////// GLOBALS //////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/* ! Link to the ObservationInterface global */
extern const sc2::ObservationInterface* obs_;

/* ! Link to the QueryInterface global */
extern sc2::QueryInterface* query_;

/* ! Link to the ActionInterface global */
extern sc2::ActionInterface* action_;

/////////////////////////////////////////////////////////////////////////////

struct IsTownHall {
    bool operator()(const sc2::Unit& unit) {
        switch ( unit.unit_type.ToType() ) {
        case sc2::UNIT_TYPEID::ZERG_HATCHERY: return true;
        case sc2::UNIT_TYPEID::ZERG_LAIR: return true;
        case sc2::UNIT_TYPEID::ZERG_HIVE: return true;
        case sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER: return true;
        case sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND: return true;
        case sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMANDFLYING: return true;
        case sc2::UNIT_TYPEID::TERRAN_PLANETARYFORTRESS: return true;
        case sc2::UNIT_TYPEID::PROTOSS_NEXUS: return true;
        default: return false;
        }
    }
};


struct IsVespeneGeyser {
    bool operator()(const sc2::Unit& unit) {
        switch ( unit.unit_type.ToType() ) {
        case sc2::UNIT_TYPEID::NEUTRAL_VESPENEGEYSER: return true;
        case sc2::UNIT_TYPEID::NEUTRAL_SPACEPLATFORMGEYSER: return true;
        case sc2::UNIT_TYPEID::NEUTRAL_PROTOSSVESPENEGEYSER: return true;
        case sc2::UNIT_TYPEID::NEUTRAL_PURIFIERVESPENEGEYSER: return true;
        case sc2::UNIT_TYPEID::NEUTRAL_SHAKURASVESPENEGEYSER: return true;
        default: return false;
        }
    }
};

struct IsMineralField {
    bool operator()(const sc2::Unit& unit) {
        switch ( unit.unit_type.ToType() ) {
        case sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD: return true;
        case sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD750: return true;
        case sc2::UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD: return true;
        case sc2::UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD750: return true;
        case sc2::UNIT_TYPEID::NEUTRAL_LABMINERALFIELD: return true;
        case sc2::UNIT_TYPEID::NEUTRAL_LABMINERALFIELD750: return true;
        default: return false;
        }
    }
};

struct IsWorker {
    bool operator()(const sc2::Unit& unit) {
        switch ( unit.unit_type.ToType() ) {
        case sc2::UNIT_TYPEID::ZERG_DRONE: return true;
        case sc2::UNIT_TYPEID::PROTOSS_PROBE: return true;
        case sc2::UNIT_TYPEID::TERRAN_SCV: return true;
        default: return false;
        }
    }
};

#endif
