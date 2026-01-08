/*
 * Lightweight compatibility shims to let early ports build without the original
 * Win32/Watcom environment. These definitions are intentionally small and will
 * be expanded as additional legacy sources move over to the portable build.
 */
#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <strings.h>

// Basic legacy-sized types used throughout the original codebase.
typedef int CELL;
typedef int COORDINATE;
typedef int DirType;
typedef int FacingType;
typedef int WindowNumberType;
typedef int TARGET;

// Constants mirrored from the legacy headers.
#ifndef MAP_CELL_W
#define MAP_CELL_W 64
#endif

#ifndef REFRESH_EOL
#define REFRESH_EOL 32767
#endif

#ifndef _MAX_FNAME
#define _MAX_FNAME 256
#endif

#ifndef _MAX_EXT
#define _MAX_EXT 256
#endif

enum RTTIType {
    RTTI_NONE = 0,
    RTTI_AIRCRAFT,
    RTTI_AIRCRAFTTYPE,
    RTTI_BUILDING,
    RTTI_BUILDINGTYPE,
};

enum MissionType {
    MISSION_NONE = -1,
    MISSION_SLEEP,
    MISSION_ATTACK,
    MISSION_MOVE,
    MISSION_RETREAT,
    MISSION_GUARD,
    MISSION_STICKY,
    MISSION_ENTER,
    MISSION_CAPTURE,
    MISSION_HARVEST,
    MISSION_GUARD_AREA,
    MISSION_RETURN,
    MISSION_STOP,
    MISSION_AMBUSH,
    MISSION_HUNT,
    MISSION_TIMED_HUNT,
    MISSION_UNLOAD,
    MISSION_SABOTAGE,
    MISSION_CONSTRUCTION,
    MISSION_DECONSTRUCTION,
    MISSION_REPAIR,
    MISSION_RESCUE,
    MISSION_MISSILE,
    MISSION_COUNT,
    MISSION_FIRST = 0
};

enum HousesType {
    HOUSE_NONE = -1,
    HOUSE_GOOD,
    HOUSE_BAD,
    HOUSE_NEUTRAL,
    HOUSE_JP,
    HOUSE_MULTI1,
    HOUSE_MULTI2,
    HOUSE_MULTI3,
    HOUSE_MULTI4,
    HOUSE_MULTI5,
    HOUSE_MULTI6,
    HOUSE_COUNT,
    HOUSE_FIRST = HOUSE_GOOD
};

#define HOUSEF_GOOD (1 << HOUSE_GOOD)
#define HOUSEF_BAD (1 << HOUSE_BAD)
#define HOUSEF_NEUTRAL (1 << HOUSE_NEUTRAL)
#define HOUSEF_JP (1 << HOUSE_JP)
#define HOUSEF_MULTI1 (1 << HOUSE_MULTI1)
#define HOUSEF_MULTI2 (1 << HOUSE_MULTI2)
#define HOUSEF_MULTI3 (1 << HOUSE_MULTI3)
#define HOUSEF_MULTI4 (1 << HOUSE_MULTI4)
#define HOUSEF_MULTI5 (1 << HOUSE_MULTI5)
#define HOUSEF_MULTI6 (1 << HOUSE_MULTI6)

enum ArmorType {
    ARMOR_NONE,
    ARMOR_WOOD,
    ARMOR_ALUMINUM,
    ARMOR_STEEL,
    ARMOR_CONCRETE,
    ARMOR_COUNT
};

enum MPHType {
    MPH_IMMOBILE = 0,
    MPH_VERY_SLOW = 5,
    MPH_KINDA_SLOW = 6,
    MPH_SLOW = 8,
    MPH_SLOW_ISH = 10,
    MPH_MEDIUM_SLOW = 12,
    MPH_MEDIUM = 18,
    MPH_MEDIUM_FAST = 30,
    MPH_MEDIUM_FASTER = 35,
    MPH_FAST = 40,
    MPH_ROCKET = 60,
    MPH_VERY_FAST = 100,
    MPH_LIGHT_SPEED = 255
};

enum WeaponType {
    WEAPON_NONE = -1,
    WEAPON_RIFLE,
    WEAPON_CHAIN_GUN,
    WEAPON_PISTOL,
    WEAPON_M16,
    WEAPON_DRAGON,
    WEAPON_FLAMETHROWER,
    WEAPON_FLAME_TONGUE,
    WEAPON_CHEMSPRAY,
    WEAPON_GRENADE,
    WEAPON_75MM,
    WEAPON_105MM,
    WEAPON_120MM,
    WEAPON_TURRET_GUN,
    WEAPON_MAMMOTH_TUSK,
    WEAPON_MLRS,
    WEAPON_155MM,
    WEAPON_M60MG,
    WEAPON_TOMAHAWK,
    WEAPON_TOW_TWO,
    WEAPON_NAPALM,
    WEAPON_OBELISK_LASER,
    WEAPON_NIKE,
    WEAPON_HONEST_JOHN,
    WEAPON_STEG,
    WEAPON_TREX,
    WEAPON_COUNT
};

enum AircraftType {
    AIRCRAFT_TRANSPORT,
    AIRCRAFT_A10,
    AIRCRAFT_HELICOPTER,
    AIRCRAFT_CARGO,
    AIRCRAFT_ORCA,
    AIRCRAFT_COUNT,
    AIRCRAFT_NONE = -1,
    AIRCRAFT_FIRST = 0
};

#define AIRCRAFTF_TRANSPORT (1L << AIRCRAFT_TRANSPORT)
#define AIRCRAFTF_A10 (1L << AIRCRAFT_A10)
#define AIRCRAFTF_HELICOPTER (1L << AIRCRAFT_HELICOPTER)
#define AIRCRAFTF_CARGO (1L << AIRCRAFT_CARGO)
#define AIRCRAFTF_ORCA (1L << AIRCRAFT_ORCA)

enum TheaterType {
    THEATER_NONE = -1,
    THEATER_DESERT,
    THEATER_JUNGLE,
    THEATER_TEMPERATE,
    THEATER_WINTER,
    THEATER_COUNT,
    THEATER_FIRST = 0
};

struct TheaterDataType {
    char Name[16];
    char Root[10];
    char Suffix[4];
};

// Legacy text identifiers used by the type constructors.
enum TextId {
    TXT_A10 = 96,
    TXT_C17 = 97,
    TXT_TRANS = 95,
    TXT_HELI = 108,
    TXT_ORCA = 109
};

// Placeholder structure for INI prerequisites.
enum StructureFlag {
    STRUCTF_NONE = 0,
    STRUCTF_HELIPAD = 1 << 0
};

class ObjectClass {
public:
    virtual ~ObjectClass() {}
};

class HouseTypeClass {
public:
    explicit HouseTypeClass(HousesType house = HOUSE_NONE) : House(house) {}
    HousesType House;
};

class HouseClass {
public:
    explicit HouseClass(HousesType house = HOUSE_NONE) : Class(house) {}

    bool Can_Build(AircraftType, int) const { return true; }
    const unsigned char* Remap_Table(bool = false, bool = false) const { return NULL; }

    static HouseClass* As_Pointer(HousesType house) {
        static HouseClass houses[HOUSE_COUNT + 1];
        static bool initialized = false;
        if (!initialized) {
            for (int i = 0; i < HOUSE_COUNT + 1; ++i) {
                houses[i] = HouseClass(static_cast<HousesType>(i - 1));
            }
            initialized = true;
        }
        int index = static_cast<int>(house) + 1;
        if (index < 0 || index >= HOUSE_COUNT + 1) return NULL;
        return &houses[index];
    }

    HouseTypeClass Class;
};

class BuildingTypeClass {
public:
    BuildingTypeClass() : ToBuild(RTTI_NONE) {}
    RTTIType ToBuild;
};

class BuildingClass : public ObjectClass {
public:
    BuildingClass()
        : IsInLimbo(false),
          House(NULL),
          Mission(MISSION_NONE),
          ActLike(0),
          Class(NULL),
          IsLeader(false) {}

    bool IsInLimbo;
    HouseClass* House;
    MissionType Mission;
    int ActLike;
    BuildingTypeClass* Class;
    bool IsLeader;
};

class BuildingCollection {
public:
    int Count() const { return 0; }
    BuildingClass* Ptr(int) const { return NULL; }
};

class TechnoTypeClass {
public:
    TechnoTypeClass(int name,
                    const char* ininame,
                    unsigned char level,
                    long pre,
                    bool is_leader,
                    bool is_scanner,
                    bool is_nominal,
                    bool is_transporter,
                    bool is_flammable,
                    bool is_crushable,
                    bool is_stealthy,
                    bool is_selectable,
                    bool is_legal_target,
                    bool is_insignificant,
                    bool is_immune,
                    bool is_theater,
                    bool is_twoshooter,
                    bool is_turret_equipped,
                    bool is_repairable,
                    bool is_buildable,
                    bool is_crew,
                    int ammo,
                    unsigned short strength,
                    MPHType maxspeed,
                    int sightrange,
                    int cost,
                    int scenario,
                    int risk,
                    int reward,
                    int ownable,
                    WeaponType primary,
                    WeaponType secondary,
                    ArmorType armor)
        : IniName(ininame),
          CameoData(NULL),
          ImageData(NULL),
          MaxStrength(strength),
          Primary(primary),
          Secondary(secondary),
          Cost(cost),
          IsTransporter(is_transporter),
          IsTwoShooter(is_twoshooter),
          IsBuildable(is_buildable),
          IsCrew(is_crew),
          IsTheater(is_theater),
          IsRepairable(is_repairable),
          Ownable(ownable),
          Level(level),
          Pre(pre),
          MaxAmmo(ammo),
          MaxSpeed(maxspeed),
          SightRange(sightrange),
          Reward(reward),
          Scenario(scenario) {
        (void)name;
        (void)is_scanner;
        (void)is_nominal;
        (void)is_flammable;
        (void)is_crushable;
        (void)is_stealthy;
        (void)is_selectable;
        (void)is_legal_target;
        (void)is_insignificant;
        (void)is_immune;
        (void)is_turret_equipped;
        (void)is_leader;
    }

    virtual ~TechnoTypeClass() {}

    virtual int Max_Passengers() const { return 0; }

    const char* IniName;
    void const* CameoData;
    void const* ImageData;
    unsigned short MaxStrength;
    WeaponType Primary;
    WeaponType Secondary;
    int Cost;
    bool IsTransporter;
    bool IsTwoShooter;
    bool IsBuildable;
    bool IsCrew;
    bool IsTheater;
    bool IsRepairable;
    int Ownable;
    unsigned char Level;
    long Pre;
    int MaxAmmo;
    MPHType MaxSpeed;
    int SightRange;
    int Reward;
    int Scenario;
};

class AircraftClass;

class AircraftTypeClass : public TechnoTypeClass {
public:
    enum RepairEnum {
        REPAIR_PERCENT = 102,
        REPAIR_STEP = 2
    };

    AircraftTypeClass(AircraftType airtype,
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
                      MissionType deforder);

    virtual RTTIType What_Am_I(void) const;

    static AircraftType From_Name(const char* name);
    static AircraftTypeClass const& As_Reference(AircraftType a) { return *Pointers[a]; }
    static void Init(TheaterType theater);
    static void One_Time(void);
    static void Prep_For_Add(void);

    virtual int Repair_Cost(void) const;
    virtual int Repair_Step(void) const;
    virtual void Dimensions(int& width, int& height) const;
    virtual bool Create_And_Place(CELL, HousesType) const;
    virtual ObjectClass* Create_One_Of(HouseClass* house) const;
    virtual short const* Occupy_List(bool placement = false) const;
    virtual short const* Overlap_List(void) const;
    virtual BuildingClass* Who_Can_Build_Me(bool intheory, bool legal, HousesType house) const;
    virtual int Max_Pips(void) const;

#ifdef SCENARIO_EDITOR
    virtual void Display(int x, int y, WindowNumberType window, HousesType house) const;
#endif

    static void const* LRotorData;
    static void const* RRotorData;

    unsigned IsFixedWing : 1;
    unsigned IsLandable : 1;
    unsigned IsRotorEquipped : 1;
    unsigned IsRotorCustom : 1;
    AircraftType Type;
    unsigned char ROT;
    MissionType Mission;

private:
    static AircraftTypeClass const* const Pointers[AIRCRAFT_COUNT];
};

class AircraftClass : public ObjectClass {
public:
    AircraftClass(AircraftType classid, HousesType house) : Type(classid), House(house) {}
    AircraftType Type;
    HousesType House;
};

// Compatibility helpers.
inline int stricmp(const char* a, const char* b) { return ::strcasecmp(a, b); }

inline void _makepath(char* path, const char*, const char* dir, const char* fname, const char* ext) {
    if (!path) return;
    const char* directory = dir ? dir : "";
    const char* filename = fname ? fname : "";
    const char* extension = ext ? ext : "";
    std::snprintf(path, _MAX_FNAME + _MAX_EXT, "%s%s%s", directory, filename, extension);
}

int Fixed_To_Cardinal(int value, int percent_fixed);
int Get_Resolution_Factor(void);

namespace MixFileClass {
void const* Retrieve(const char* filename);
}

extern BuildingCollection Buildings;
extern TheaterDataType Theaters[THEATER_COUNT];
extern TheaterType LastTheater;
