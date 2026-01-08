#include "legacy_compat.h"

#include <cstdio>

void const* AircraftTypeClass::LRotorData = NULL;
void const* AircraftTypeClass::RRotorData = NULL;

// A-10 attack plane
static AircraftTypeClass const AttackPlane(
    AIRCRAFT_A10,          // What kind of aircraft is this.
    TXT_A10,               // Translated text number for aircraft.
    "A10",                 // INI name of aircraft.
    99,                    // Build level.
    STRUCTF_NONE,          // Building prerequisite.
    false,                 // Is a leader type?
    false,                 // Does it fire a pair of shots in quick succession?
    false,                 // Is this a typical transport vehicle?
    true,                  // Fixed wing aircraft?
    false,                 // Equipped with a rotor?
    false,                 // Custom rotor sets for each facing?
    false,                 // Can this aircraft land on clear terrain?
    false,                 // Can the aircraft be crushed by a tracked vehicle?
    true,                  // Is it invisible on radar?
    false,                 // Can the player select it so as to give it orders?
    true,                  // Can it be assigned as a target for attack.
    false,                 // Is it insignificant (won't be announced)?
    false,                 // Is it immune to normal combat damage?
    false,                 // Theater specific graphic image?
    false,                 // Can it be repaired in a repair facility?
    false,                 // Can the player construct or order this unit?
    true,                  // Is there a crew inside?
    3,                     // Number of shots it has (default).
    60,                    // The strength of this unit.
    0,                     // The range that it reveals terrain around itself.
    800,                   // Credit cost to construct.
    0,                     // The scenario this becomes available.
    10, 1,                 // Risk, reward when calculating AI.
    HOUSEF_MULTI1 | HOUSEF_MULTI2 | HOUSEF_MULTI3 | HOUSEF_MULTI4 | HOUSEF_MULTI5 | HOUSEF_MULTI6 | HOUSEF_JP |
        HOUSEF_GOOD | HOUSEF_BAD,  // Who can own this aircraft type.
    WEAPON_NAPALM, WEAPON_NONE,
    ARMOR_ALUMINUM,  // Armor type of this aircraft.
    MPH_FAST,        // Maximum speed of aircraft.
    5,               // Rate of turn.
    MISSION_HUNT     // Default mission for aircraft.
);

// Transport helicopter.
static AircraftTypeClass const TransportHeli(
    AIRCRAFT_TRANSPORT,  // What kind of aircraft is this.
    TXT_TRANS,           // Translated text number for aircraft.
    "TRAN",              // INI name of aircraft.
    6,                   // Build level.
    STRUCTF_HELIPAD,     // Building prerequisite.
    false,               // Is a leader type?
    false,               // Does it fire a pair of shots in quick succession?
    true,                // Is this a typical transport vehicle?
    false,               // Fixed wing aircraft?
    true,                // Equipped with a rotor?
    true,                // Custom rotor sets for each facing?
    true,                // Can this aircraft land on clear terrain?
    false,               // Can the aircraft be crushed by a tracked vehicle?
    true,                // Is it invisible on radar?
    true,                // Can the player select it so as to give it orders?
    true,                // Can it be assigned as a target for attack.
    false,               // Is it insignificant (won't be announced)?
    false,               // Theater specific graphic image?
    false,               // Is it equipped with a combat turret?
    false,               // Can it be repaired in a repair facility?
    true,                // Can the player construct or order this unit?
    true,                // Is there a crew inside?
    0,                   // Number of shots it has (default).
    90,                  // The strength of this unit.
    0,                   // The range that it reveals terrain around itself.
    1500,                // Credit cost to construct.
    98,                  // The scenario this becomes available.
    10, 80,              // Risk, reward when calculating AI.
    HOUSEF_MULTI1 | HOUSEF_MULTI2 | HOUSEF_MULTI3 | HOUSEF_MULTI4 | HOUSEF_MULTI5 | HOUSEF_MULTI6 | HOUSEF_JP |
        HOUSEF_BAD | HOUSEF_GOOD,  // Who can own this aircraft type.
    WEAPON_NONE, WEAPON_NONE,
    ARMOR_ALUMINUM,   // Armor type of this aircraft.
    MPH_MEDIUM_FAST,  // Maximum speed of aircraft.
    5,                // Rate of turn.
    MISSION_HUNT      // Default mission for aircraft.
);

// Apache attack helicopter.
static AircraftTypeClass const AttackHeli(
    AIRCRAFT_HELICOPTER,  // What kind of aircraft is this.
    TXT_HELI,             // Translated text number for aircraft.
    "HELI",               // INI name of aircraft.
    6,                    // Build level.
    STRUCTF_HELIPAD,      // Building prerequisite.
    true,                 // Is a leader type?
    true,                 // Does it fire a pair of shots in quick succession?
    false,                // Is this a typical transport vehicle?
    false,                // Fixed wing aircraft?
    true,                 // Equipped with a rotor?
    false,                // Custom rotor sets for each facing?
    false,                // Can this aircraft land on clear terrain?
    false,                // Can the aircraft be crushed by a tracked vehicle?
    true,                 // Is it invisible on radar?
    true,                 // Can the player select it so as to give it orders?
    true,                 // Can it be assigned as a target for attack.
    false,                // Is it insignificant (won't be announced)?
    false,                // Is it immune to normal combat damage?
    false,                // Theater specific graphic image?
    false,                // Can it be repaired in a repair facility?
    true,                 // Can the player construct or order this unit?
    true,                 // Is there a crew inside?
    15,                   // Number of shots it has (default).
    125,                  // The strength of this unit.
    0,                    // The range that it reveals terrain around itself.
    1200,                 // Credit cost to construct.
    10,                   // The scenario this becomes available.
    10, 80,               // Risk, reward when calculating AI.
    HOUSEF_MULTI1 | HOUSEF_MULTI2 | HOUSEF_MULTI3 | HOUSEF_MULTI4 | HOUSEF_MULTI5 | HOUSEF_MULTI6 | HOUSEF_JP |
        HOUSEF_BAD,  // Who can own this aircraft type.
    WEAPON_CHAIN_GUN, WEAPON_NONE,
    ARMOR_STEEL,  // Armor type of this aircraft.
    MPH_FAST,     // Maximum speed of aircraft.
    4,            // Rate of turn.
    MISSION_HUNT  // Default mission for aircraft.
);

// Orca attack helicopter.
static AircraftTypeClass const OrcaHeli(
    AIRCRAFT_ORCA,        // What kind of aircraft is this.
    TXT_ORCA,             // Translated text number for aircraft.
    "ORCA",               // INI name of aircraft.
    6,                    // Build level.
    STRUCTF_HELIPAD,      // Building prerequisite.
    true,                 // Is a leader type?
    true,                 // Does it fire a pair of shots in quick succession?
    false,                // Is this a typical transport vehicle?
    false,                // Fixed wing aircraft?
    false,                // Equipped with a rotor?
    false,                // Custom rotor sets for each facing?
    false,                // Can this aircraft land on clear terrain?
    false,                // Can the aircraft be crushed by a tracked vehicle?
    true,                 // Is it invisible on radar?
    true,                 // Can the player select it so as to give it orders?
    true,                 // Can it be assigned as a target for attack.
    false,                // Is it insignificant (won't be announced)?
    false,                // Is it immune to normal combat damage?
    false,                // Theater specific graphic image?
    false,                // Can it be repaired in a repair facility?
    true,                 // Can the player construct or order this unit?
    true,                 // Is there a crew inside?
    6,                    // Number of shots it has (default).
    125,                  // The strength of this unit.
    0,                    // The range that it reveals terrain around itself.
    1200,                 // Credit cost to construct.
    10,                   // The scenario this becomes available.
    10, 80,               // Risk, reward when calculating AI.
    HOUSEF_MULTI1 | HOUSEF_MULTI2 | HOUSEF_MULTI3 | HOUSEF_MULTI4 | HOUSEF_MULTI5 | HOUSEF_MULTI6 | HOUSEF_JP |
        HOUSEF_GOOD,  // Who can own this aircraft type.
    WEAPON_DRAGON, WEAPON_NONE,
    ARMOR_STEEL,  // Armor type of this aircraft.
    MPH_FAST,     // Maximum speed of aircraft.
    4,            // Rate of turn.
    MISSION_HUNT  // Default mission for aircraft.
);

// C-17 transport plane.
static AircraftTypeClass const CargoPlane(
    AIRCRAFT_CARGO,       // What kind of aircraft is this.
    TXT_C17,              // Translated text number for aircraft.
    "C17",                // INI name of aircraft.
    99,                   // Build level.
    STRUCTF_NONE,         // Building prerequisite.
    false,                // Is a leader type?
    false,                // Does it fire a pair of shots in quick succession?
    true,                 // Is this a typical transport vehicle?
    true,                 // Fixed wing aircraft?
    false,                // Equipped with a rotor?
    false,                // Custom rotor sets for each facing?
    false,                // Can this aircraft land on clear terrain?
    false,                // Can the aircraft be crushed by a tracked vehicle?
    true,                 // Is it invisible on radar?
    false,                // Can the player select it so as to give it orders?
    false,                // Can it be assigned as a target for attack.
    false,                // Is it insignificant (won't be announced)?
    false,                // Is it immune to normal combat damage?
    false,                // Theater specific graphic image?
    false,                // Can it be repaired in a repair facility?
    false,                // Can the player construct or order this unit?
    true,                 // Is there a crew inside?
    0,                    // Number of shots it has (default).
    25,                   // The strength of this unit.
    0,                    // The range that it reveals terrain around itself.
    800,                  // Credit cost to construct.
    0,                    // The scenario this becomes available.
    10, 1,                // Risk, reward when calculating AI.
    HOUSEF_MULTI1 | HOUSEF_MULTI2 | HOUSEF_MULTI3 | HOUSEF_MULTI4 | HOUSEF_MULTI5 | HOUSEF_MULTI6 | HOUSEF_JP |
        HOUSEF_GOOD | HOUSEF_BAD,  // Who can own this aircraft type.
    WEAPON_NONE, WEAPON_NONE,
    ARMOR_ALUMINUM,  // Armor type of this aircraft.
    MPH_FAST,        // Maximum speed of aircraft.
    5,               // Rate of turn.
    MISSION_HUNT     // Default mission for aircraft.
);

AircraftTypeClass const* const AircraftTypeClass::Pointers[AIRCRAFT_COUNT] = {
    &TransportHeli,
    &AttackPlane,
    &AttackHeli,
    &CargoPlane,
    &OrcaHeli,
};

AircraftTypeClass::AircraftTypeClass(AircraftType airtype,
                                     int name,
                                     const char* ininame,
                                     unsigned char level,
                                     long pre,
                                     bool is_leader,
                                     bool is_twoshooter,
                                     bool is_transporter,
                                     bool is_fixedwing,
                                     bool is_rotorequipped,
                                     bool is_rotorcustom,
                                     bool is_landable,
                                     bool is_crushable,
                                     bool is_stealthy,
                                     bool is_selectable,
                                     bool is_legal_target,
                                     bool is_insignificant,
                                     bool is_immune,
                                     bool is_theater,
                                     bool is_repairable,
                                     bool is_buildable,
                                     bool is_crew,
                                     int ammo,
                                     unsigned short strength,
                                     int sightrange,
                                     int cost,
                                     int scenario,
                                     int risk,
                                     int reward,
                                     int ownable,
                                     WeaponType primary,
                                     WeaponType secondary,
                                     ArmorType armor,
                                     MPHType maxspeed,
                                     int rot,
                                     MissionType deforder)
    : TechnoTypeClass(name,
                      ininame,
                      level,
                      pre,
                      is_leader,
                      false,
                      false,
                      is_transporter,
                      false,
                      is_crushable,
                      is_stealthy,
                      is_selectable,
                      is_legal_target,
                      is_insignificant,
                      is_immune,
                      is_theater,
                      is_twoshooter,
                      false,
                      is_repairable,
                      is_buildable,
                      is_crew,
                      ammo,
                      strength,
                      maxspeed,
                      sightrange,
                      cost,
                      scenario,
                      risk,
                      reward,
                      ownable,
                      primary,
                      secondary,
                      armor) {
    IsRotorEquipped = is_rotorequipped;
    IsRotorCustom = is_rotorcustom;
    IsLandable = is_landable;
    IsFixedWing = is_fixedwing;
    Type = airtype;
    ROT = rot;
    Mission = deforder;
}

AircraftType AircraftTypeClass::From_Name(const char* name) {
    if (name) {
        for (int idx = AIRCRAFT_FIRST; idx < AIRCRAFT_COUNT; ++idx) {
            AircraftType classid = static_cast<AircraftType>(idx);
            if (stricmp(Pointers[idx]->IniName, name) == 0) {
                return classid;
            }
        }
    }
    return AIRCRAFT_NONE;
}

void AircraftTypeClass::One_Time(void) {
    for (int idx = AIRCRAFT_FIRST; idx < AIRCRAFT_COUNT; ++idx) {
        char fullname[_MAX_FNAME + _MAX_EXT];
        char buffer[_MAX_FNAME];
        AircraftTypeClass const& uclass = As_Reference(static_cast<AircraftType>(idx));

        if (Get_Resolution_Factor()) {
            std::snprintf(buffer, sizeof(buffer), "%sICNH", uclass.IniName);
        } else {
            std::snprintf(buffer, sizeof(buffer), "%sICON", uclass.IniName);
        }
        _makepath(fullname, NULL, NULL, buffer, ".SHP");
        ((void const*&)uclass.CameoData) = MixFileClass::Retrieve(fullname);

        _makepath(fullname, NULL, NULL, uclass.IniName, ".SHP");
        ((void const*&)uclass.ImageData) = MixFileClass::Retrieve(fullname);
    }

    LRotorData = MixFileClass::Retrieve("LROTOR.SHP");
    RRotorData = MixFileClass::Retrieve("RROTOR.SHP");
}

ObjectClass* AircraftTypeClass::Create_One_Of(HouseClass* house) const {
    return new AircraftClass(Type, house ? house->Class.House : HOUSE_NONE);
}

short const* AircraftTypeClass::Occupy_List(bool) const {
    static short const list[] = {0, REFRESH_EOL};
    return list;
}

short const* AircraftTypeClass::Overlap_List(void) const {
    static short const list[] = {
        -(MAP_CELL_W - 1), -MAP_CELL_W, -(MAP_CELL_W + 1), -1, 1, (MAP_CELL_W - 1), MAP_CELL_W, (MAP_CELL_W + 1),
        REFRESH_EOL};
    return list;
}

BuildingClass* AircraftTypeClass::Who_Can_Build_Me(bool, bool legal, HousesType house) const {
    BuildingClass* anybuilding = NULL;
    for (int index = 0; index < Buildings.Count(); index++) {
        BuildingClass* building = Buildings.Ptr(index);

        if (building && !building->IsInLimbo && building->House && building->House->Class.House == house &&
            building->Mission != MISSION_DECONSTRUCTION && ((1L << building->ActLike) & Ownable) &&
            (!legal || building->House->Can_Build(Type, building->ActLike)) &&
            building->Class && building->Class->ToBuild == RTTI_AIRCRAFTTYPE) {
            if (building->IsLeader) return building;
            anybuilding = building;
        }
    }
    return anybuilding;
}

int AircraftTypeClass::Repair_Cost(void) const {
    return Fixed_To_Cardinal(Cost / (MaxStrength / REPAIR_STEP), REPAIR_PERCENT);
}

int AircraftTypeClass::Repair_Step(void) const { return REPAIR_STEP; }

int AircraftTypeClass::Max_Pips(void) const {
    if (IsTransporter) {
        return Max_Passengers();
    }
    if (Primary != WEAPON_NONE) {
        return 5;
    }
    return 0;
}

bool AircraftTypeClass::Create_And_Place(CELL, HousesType) const { return false; }

void AircraftTypeClass::Init(TheaterType theater) {
    if (theater != LastTheater) {
        if (Get_Resolution_Factor()) {
            char buffer[_MAX_FNAME];
            char fullname[_MAX_FNAME + _MAX_EXT];

            for (int idx = AIRCRAFT_FIRST; idx < AIRCRAFT_COUNT; ++idx) {
                AircraftTypeClass const& uclass = As_Reference(static_cast<AircraftType>(idx));

                ((void const*&)uclass.CameoData) = NULL;

                std::snprintf(buffer, sizeof(buffer), "%.4sICNH", uclass.IniName);
                _makepath(fullname, NULL, NULL, buffer, Theaters[theater].Suffix);
                void const* cameo_ptr = MixFileClass::Retrieve(fullname);
                if (cameo_ptr) {
                    ((void const*&)uclass.CameoData) = cameo_ptr;
                }
            }
        }
    }
}

void AircraftTypeClass::Dimensions(int& width, int& height) const {
    width = 21;
    height = 20;
}

RTTIType AircraftTypeClass::What_Am_I(void) const { return RTTI_AIRCRAFTTYPE; }
