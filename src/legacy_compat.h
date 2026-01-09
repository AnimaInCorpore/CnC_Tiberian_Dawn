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
#include <string>
#include <vector>

// Legacy typedefs/macros used by older sources.
#ifndef __cdecl
#define __cdecl
#endif

typedef void VOID;
typedef unsigned long ULONG;
typedef long LONG;
typedef unsigned int TextPrintType;

#ifndef WW_ERROR
#define WW_ERROR (-1)
#endif

#ifndef READ
#define READ 1
#endif

#ifndef WRITE
#define WRITE 2
#endif

// Basic legacy-sized types used throughout the original codebase.
typedef int CELL;
typedef int COORDINATE;
typedef int DirType;
typedef int VolType;
typedef int WindowNumberType;
typedef int TARGET;

// Common palette indices used by the legacy UI/terrain tables.
#ifndef BLUE
#define BLUE 1
#endif
#ifndef GREEN
#define GREEN 2
#endif
#ifndef GREY
#define GREY 7
#endif
#ifndef DKGREY
#define DKGREY GREY
#endif

// Constants mirrored from the legacy headers.
#ifndef MAP_CELL_W
#define MAP_CELL_W 64
#endif

#ifndef MAP_CELL_H
#define MAP_CELL_H 64
#endif

#ifndef MAP_CELL_TOTAL
#define MAP_CELL_TOTAL (MAP_CELL_W * MAP_CELL_H)
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
    RTTI_ANIM,
    RTTI_BUILDING,
    RTTI_BUILDINGTYPE,
    RTTI_BULLET,
    RTTI_INFANTRYTYPE,
    RTTI_UNITTYPE,
    RTTI_ABSTRACTTYPE,
    RTTI_ANIMTYPE,
    RTTI_BULLETTYPE,
};

enum ResultType {
    RESULT_NONE = 0,
    RESULT_DAMAGED = 1,
    RESULT_DESTROYED = 2,
};

enum MarkType {
    MARK_UP,
    MARK_DOWN,
    MARK_CHANGE,
    MARK_OVERLAP_DOWN,
    MARK_OVERLAP_UP,
};

enum LayerType {
    LAYER_NONE = -1,
    LAYER_GROUND,
    LAYER_AIR,
    LAYER_TOP,

    LAYER_COUNT,
    LAYER_FIRST = 0
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

typedef enum LandType {
    LAND_CLEAR,     // "Clear" terrain.
    LAND_ROAD,      // Road terrain.
    LAND_WATER,     // Water.
    LAND_ROCK,      // Impassable rock.
    LAND_WALL,      // Wall (blocks movement).
    LAND_TIBERIUM,  // Tiberium field.
    LAND_BEACH,     // Beach terrain.

    LAND_COUNT
} LandType;

enum SourceType {
    SOURCE_NORTH,
    SOURCE_EAST,
    SOURCE_SOUTH,
    SOURCE_WEST,
    SOURCE_SHIPPING,
    SOURCE_BEACH,
    SOURCE_AIR,
    SOURCE_VISIBLE,
    SOURCE_ENEMY_BASE,
    SOURCE_HOME_BASE,
    SOURCE_OCEAN,
    SOURCE_COUNT
};

typedef enum StructType {
    STRUCT_NONE = -1,
    STRUCT_WEAP,
    STRUCT_GTOWER,
    STRUCT_ATOWER,
    STRUCT_OBELISK,
    STRUCT_RADAR,
    STRUCT_TURRET,
    STRUCT_CONST,
    STRUCT_REFINERY,
    STRUCT_STORAGE,
    STRUCT_HELIPAD,
    STRUCT_SAM,
    STRUCT_AIRSTRIP,
    STRUCT_POWER,
    STRUCT_ADVANCED_POWER,
    STRUCT_HOSPITAL,
    STRUCT_BARRACKS,
    STRUCT_TANKER,
    STRUCT_REPAIR,
    STRUCT_BIO_LAB,
    STRUCT_HAND,
    STRUCT_TEMPLE,
    STRUCT_EYE,
    STRUCT_MISSION,

    STRUCT_V01,
    STRUCT_V02,
    STRUCT_V03,
    STRUCT_V04,
    STRUCT_V05,
    STRUCT_V06,
    STRUCT_V07,
    STRUCT_V08,
    STRUCT_V09,
    STRUCT_V10,
    STRUCT_V11,
    STRUCT_V12,
    STRUCT_V13,
    STRUCT_V14,
    STRUCT_V15,
    STRUCT_V16,
    STRUCT_V17,
    STRUCT_V18,
    STRUCT_PUMP,
    STRUCT_V20,
    STRUCT_V21,
    STRUCT_V22,
    STRUCT_V23,
    STRUCT_V24,
    STRUCT_V25,
    STRUCT_V26,
    STRUCT_V27,
    STRUCT_V28,
    STRUCT_V29,
    STRUCT_V30,
    STRUCT_V31,
    STRUCT_V32,
    STRUCT_V33,
    STRUCT_V34,
    STRUCT_V35,
    STRUCT_V36,
    STRUCT_V37,
#ifdef OBSOLETE
    STRUCT_ROAD,
#endif
    STRUCT_SANDBAG_WALL,
    STRUCT_CYCLONE_WALL,
    STRUCT_BRICK_WALL,
    STRUCT_BARBWIRE_WALL,
    STRUCT_WOOD_WALL,

    STRUCT_COUNT,
    STRUCT_FIRST = 0
} StructType;

inline StructType operator++(StructType& value, int) {
    int next = static_cast<int>(value) + 1;
    if (next >= static_cast<int>(STRUCT_COUNT)) return value;
    value = static_cast<StructType>(next);
    return value;
}

template <typename T>
class DynamicVectorClass {
public:
    DynamicVectorClass() : Data() {}

    int Count() const { return static_cast<int>(Data.size()); }
    void Clear() { Data.clear(); }
    void Add(T const& value) { Data.push_back(value); }
    void Delete(int index) {
        if (index < 0 || index >= Count()) return;
        Data.erase(Data.begin() + static_cast<typename std::vector<T>::difference_type>(index));
    }

    T& operator[](int index) { return Data[static_cast<size_t>(index)]; }
    T const& operator[](int index) const { return Data[static_cast<size_t>(index)]; }

private:
    std::vector<T> Data;
};

// Minimal UI/text rendering shims (no-op until display port lands).
enum TextPrintFlagsEnum {
    TPF_6PT_GRAD = 1u << 0,
    TPF_BRIGHT_COLOR = 1u << 1,
    TPF_NOSHADOW = 1u << 2,
    TPF_USE_GRAD_PAL = 1u << 3
};

enum TextColorEnum {
    BLACK = 0,
    LTGREY = 7,
    WHITE = 15,
    TBLACK = 0,
    CC_GREEN = 2
};

class GraphicPageClass {
public:
    void Draw_Rect(int, int, int, int, int) {}
    void Fill_Rect(int, int, int, int, int) {}
};

class GraphicBufferClass : public GraphicPageClass {
public:
    GraphicBufferClass() : Width(320), Height(200) {}
    GraphicBufferClass(int width, int height) : Width(width), Height(height) {}

    int Get_Width() const { return Width; }
    int Get_Height() const { return Height; }

private:
    int Width;
    int Height;
};

extern GraphicPageClass* LogicPage;
extern GraphicBufferClass SeenBuff;

inline void Set_Logic_Page(GraphicPageClass& page) { LogicPage = &page; }

void Hide_Mouse();
void Show_Mouse();

void Dialog_Box(int x, int y, int w, int h);
void Draw_Caption(int text, int x, int y, int w);

int String_Pixel_Width(char const* text);
extern int FontHeight;
extern int FontYSpacing;

void Format_Window_String(char* text, int max_width, int& out_width, int& out_height);

char const* Text_String(int text_id);

void Fancy_Text_Print(char const* text,
                      int x,
                      int y,
                      int fore,
                      int back,
                      TextPrintType flags);
inline void Fancy_Text_Print(int text_id, int x, int y, int fore, int back, TextPrintType flags) {
    Fancy_Text_Print(Text_String(text_id), x, y, fore, back, flags);
}

void Conquer_Clip_Text_Print(char const* text,
                             int x,
                             int y,
                             int fore,
                             int back,
                             TextPrintType flags,
                             int width,
                             int const* tabs);

typedef int KeyNumType;
enum KeyNumConstants {
    KN_NONE = 0,
    KN_ESC = 27,
    KN_RETURN = 13,
    KN_LEFT = 1000,
    KN_RIGHT = 1001,
    KN_BUTTON = 1u << 15
};

enum GameType {
    GAME_NORMAL = 0,
    GAME_OTHER = 1
};

extern GameType GameToPlay;
void Call_Back();
bool Main_Loop();

struct SurfaceManager {
    bool SurfacesRestored;
};

extern SurfaceManager AllSurfaces;

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

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

enum MoveType {
    MOVE_OK,
    MOVE_CLOAK,
    MOVE_MOVING_BLOCK,
    MOVE_DESTROYABLE,
    MOVE_TEMP,
    MOVE_NO,
    MOVE_COUNT
};

enum FacingType {
    FACING_NONE = -1,
    FACING_N,
    FACING_NE,
    FACING_E,
    FACING_SE,
    FACING_S,
    FACING_SW,
    FACING_W,
    FACING_NW,
    FACING_COUNT,
    FACING_FIRST = 0
};

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

typedef enum ImpactType {
    IMPACT_NONE,   // No movement (of significance) occurred.
    IMPACT_NORMAL, // Some (non eventful) movement occurred.
    IMPACT_EDGE    // The edge of the world was reached.
} ImpactType;

typedef enum BulletType {
    BULLET_NONE = -1,
    BULLET_SNIPER,        // Sniper bullet.
    BULLET_BULLET,        // Small arms
    BULLET_APDS,          // Armor piercing projectile.
    BULLET_HE,            // High explosive shell.
    BULLET_SSM,           // Surface to surface small missile type.
    BULLET_SSM2,          // MLRS missile.
    BULLET_SAM,           // Fast homing anti-aircraft missile.
    BULLET_TOW,           // TOW anti-vehicle short range missile.
    BULLET_FLAME,         // Flame thrower flame.
    BULLET_CHEMSPRAY,     // Chemical weapon spray.
    BULLET_NAPALM,        // Napalm bomblet.
    BULLET_GRENADE,       // Hand tossed grenade.
    BULLET_LASER,         // Laser beam from obelisk
    BULLET_NUKE_UP,       // Nuclear Missile on its way down
    BULLET_NUKE_DOWN,     // Nuclear Missile on its way up
    BULLET_HONEST_JOHN,   // SSM with napalm warhead.
    BULLET_SPREADFIRE,    // Chain gun bullets.
    BULLET_HEADBUTT,      // Stegosaurus, Triceratops head butt
    BULLET_TREXBITE,      // Tyrannosaurus Rex's bite - especially bad for infantry

    BULLET_COUNT,
    BULLET_FIRST = 0
} BulletType;

class FacingClass {
public:
    FacingClass() : CurrentFacing(0), DesiredFacing(0) {}
    explicit FacingClass(DirType dir) : CurrentFacing(dir), DesiredFacing(dir) {}

    operator DirType(void) const { return CurrentFacing; }
    DirType Current(void) const { return CurrentFacing; }
    DirType Desired(void) const { return DesiredFacing; }

    int Set_Desired(DirType facing) {
        DesiredFacing = facing;
        return 0;
    }
    int Set_Current(DirType facing) {
        CurrentFacing = facing;
        return 0;
    }
    void Set(DirType facing) {
        Set_Current(facing);
        Set_Desired(facing);
    }

    DirType Get(void) const { return CurrentFacing; }
    int Is_Rotating(void) const { return DesiredFacing != CurrentFacing; }
    int Difference(void) const { return static_cast<signed char>(DesiredFacing - CurrentFacing); }
    int Difference(DirType facing) const { return static_cast<signed char>(facing - CurrentFacing); }

    int Rotation_Adjust(int) { return 0; }

private:
    DirType CurrentFacing;
    DirType DesiredFacing;
};

class FlyClass {
public:
    FlyClass() : SpeedAccum(0), SpeedAdd(MPH_IMMOBILE) {}

    void Fly_Speed(int, MPHType maximum) { SpeedAdd = maximum; }
    ImpactType Physics(COORDINATE&, DirType) { return IMPACT_NONE; }
    MPHType Get_Speed(void) const { return SpeedAdd; }

    void Code_Pointers(void) {}
    void Decode_Pointers(void) {}

private:
    unsigned SpeedAccum;
    MPHType SpeedAdd;
};

class FileClass;

class FuseClass {
public:
    FuseClass() : Timer(0), Arming(0), HeadTo(0), Proximity(0) {}

    void Arm_Fuse(COORDINATE, COORDINATE target, int time = 0xFF, int arming = 0) {
        HeadTo = target;
        Timer = static_cast<unsigned char>(time);
        Arming = static_cast<unsigned char>(arming);
        Proximity = 0;
    }
    bool Fuse_Checkup(COORDINATE) { return false; }
    void Fuse_Write(FileClass&) {}
    void Fuse_Read(FileClass&) {}
    COORDINATE Fuse_Target(void) { return HeadTo; }

    void Code_Pointers(void) {}
    void Decode_Pointers(void) {}

    unsigned char Timer;

private:
    unsigned char Arming;
    COORDINATE HeadTo;
    short Proximity;
};

typedef enum WarheadType {
    WARHEAD_NONE = -1,
    WARHEAD_SA,             // Small arms -- good against infantry.
    WARHEAD_HE,             // High explosive -- good against buildings & infantry.
    WARHEAD_AP,             // Amor piercing -- good against armor.
    WARHEAD_FIRE,           // Incendiary -- Good against flammables.
    WARHEAD_LASER,          // Light Amplification of Stimulated Emission of Radiation.
    WARHEAD_PB,             // Particle beam (neutron beam).
    WARHEAD_FIST,           // punching in hand-to-hand combat
    WARHEAD_FOOT,           // kicking in hand-to-hand combat
    WARHEAD_HOLLOW_POINT,   // Sniper bullet type.
    WARHEAD_SPORE,          // Spores from blossom tree - affect infantry only
    WARHEAD_HEADBUTT,       // Other dinosaurs butt into people
    WARHEAD_FEEDME,         // T-Rex eats people, hurts vehicles/buildings

    WARHEAD_COUNT
} WarheadType;

struct WarheadTypeClass {
    int SpreadFactor;
    bool IsWallDestroyer;
    bool IsWoodDestroyer;
    bool IsTiberiumDestroyer;
    int Modifier[ARMOR_COUNT];
};

extern WarheadTypeClass const Warheads[WARHEAD_COUNT];

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

struct WeaponTypeClass {
    BulletType Fires;
    int Attack;
    int ROF;
    int Range;
    int Sound;
    int Explosion;
};

extern WeaponTypeClass const Weapons[WEAPON_COUNT];

extern char const* SourceName[SOURCE_COUNT];
extern COORDINATE const StoppingCoordAbs[5];
extern unsigned char const Pixel2Lepton[24];
extern CELL const AdjacentCell[FACING_COUNT];
extern COORDINATE const AdjacentCoord[FACING_COUNT];
extern unsigned char const Facing8[256];
extern unsigned char const Facing32[256];

struct GroundType {
    int RadarColor;
    unsigned char Speed[7];
    bool IsBuildable;
};

extern GroundType const Ground[LAND_COUNT];

extern unsigned char const RemapYellow[256];
extern unsigned char const RemapRed[256];
extern unsigned char const RemapBlueGreen[256];
extern unsigned char const RemapOrange[256];
extern unsigned char const RemapGreen[256];
extern unsigned char const RemapBlue[256];
extern unsigned char const RemapNone[256];

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

#define THEATERF_DESERT (1 << THEATER_DESERT)
#define THEATERF_JUNGLE (1 << THEATER_JUNGLE)
#define THEATERF_TEMPERATE (1 << THEATER_TEMPERATE)
#define THEATERF_WINTER (1 << THEATER_WINTER)

typedef enum TemplateType {
    TEMPLATE_CLEAR1,
    TEMPLATE_WATER, // This must be the first non-clear template.
    TEMPLATE_WATER2,
    TEMPLATE_SHORE1,
    TEMPLATE_SHORE2,
    TEMPLATE_SHORE3,
    TEMPLATE_SHORE4,
    TEMPLATE_SHORE5,
    TEMPLATE_SHORE11,
    TEMPLATE_SHORE12,
    TEMPLATE_SHORE13,
    TEMPLATE_SHORE14,
    TEMPLATE_SHORE15,
    TEMPLATE_SLOPE1,
    TEMPLATE_SLOPE2,
    TEMPLATE_SLOPE3,
    TEMPLATE_SLOPE4,
    TEMPLATE_SLOPE5,
    TEMPLATE_SLOPE6,
    TEMPLATE_SLOPE7,
    TEMPLATE_SLOPE8,
    TEMPLATE_SLOPE9,
    TEMPLATE_SLOPE10,
    TEMPLATE_SLOPE11,
    TEMPLATE_SLOPE12,
    TEMPLATE_SLOPE13,
    TEMPLATE_SLOPE14,
    TEMPLATE_SLOPE15,
    TEMPLATE_SLOPE16,
    TEMPLATE_SLOPE17,
    TEMPLATE_SLOPE18,
    TEMPLATE_SLOPE19,
    TEMPLATE_SLOPE20,
    TEMPLATE_SLOPE21,
    TEMPLATE_SLOPE22,
    TEMPLATE_SLOPE23,
    TEMPLATE_SLOPE24,
    TEMPLATE_SLOPE25,
    TEMPLATE_SLOPE26,
    TEMPLATE_SLOPE27,
    TEMPLATE_SLOPE28,
    TEMPLATE_SLOPE29,
    TEMPLATE_SLOPE30,
    TEMPLATE_SLOPE31,
    TEMPLATE_SLOPE32,
    TEMPLATE_SLOPE33,
    TEMPLATE_SLOPE34,
    TEMPLATE_SLOPE35,
    TEMPLATE_SLOPE36,
    TEMPLATE_SLOPE37,
    TEMPLATE_SLOPE38,
    TEMPLATE_SHORE32,
    TEMPLATE_SHORE33,
    TEMPLATE_SHORE20,
    TEMPLATE_SHORE21,
    TEMPLATE_SHORE22,
    TEMPLATE_SHORE23,
    TEMPLATE_BRUSH1,
    TEMPLATE_BRUSH2,
    TEMPLATE_BRUSH3,
    TEMPLATE_BRUSH4,
    TEMPLATE_BRUSH5,
    TEMPLATE_BRUSH6,
    TEMPLATE_BRUSH7,
    TEMPLATE_BRUSH8,
    TEMPLATE_BRUSH9,
    TEMPLATE_BRUSH10,
    TEMPLATE_PATCH1,
    TEMPLATE_PATCH2,
    TEMPLATE_PATCH3,
    TEMPLATE_PATCH4,
    TEMPLATE_PATCH5,
    TEMPLATE_PATCH6,
    TEMPLATE_PATCH7,
    TEMPLATE_PATCH8,
    TEMPLATE_SHORE16,
    TEMPLATE_SHORE17,
    TEMPLATE_SHORE18,
    TEMPLATE_SHORE19,
    TEMPLATE_PATCH13,
    TEMPLATE_PATCH14,
    TEMPLATE_PATCH15,
    TEMPLATE_BOULDER1,
    TEMPLATE_BOULDER2,
    TEMPLATE_BOULDER3,
    TEMPLATE_BOULDER4,
    TEMPLATE_BOULDER5,
    TEMPLATE_BOULDER6,
    TEMPLATE_SHORE6,
    TEMPLATE_SHORE7,
    TEMPLATE_SHORE8,
    TEMPLATE_SHORE9,
    TEMPLATE_SHORE10,

    TEMPLATE_ROAD1,
    TEMPLATE_ROAD2,
    TEMPLATE_ROAD3,
    TEMPLATE_ROAD4,
    TEMPLATE_ROAD5,
    TEMPLATE_ROAD6,
    TEMPLATE_ROAD7,
    TEMPLATE_ROAD8,
    TEMPLATE_ROAD9,
    TEMPLATE_ROAD10,
    TEMPLATE_ROAD11,
    TEMPLATE_ROAD12,
    TEMPLATE_ROAD13,
    TEMPLATE_ROAD14,
    TEMPLATE_ROAD15,
    TEMPLATE_ROAD16,
    TEMPLATE_ROAD17,
    TEMPLATE_ROAD18,
    TEMPLATE_ROAD19,
    TEMPLATE_ROAD20,
    TEMPLATE_ROAD21,
    TEMPLATE_ROAD22,
    TEMPLATE_ROAD23,
    TEMPLATE_ROAD24,
    TEMPLATE_ROAD25,
    TEMPLATE_ROAD26,
    TEMPLATE_ROAD27,
    TEMPLATE_ROAD28,
    TEMPLATE_ROAD29,
    TEMPLATE_ROAD30,
    TEMPLATE_ROAD31,
    TEMPLATE_ROAD32,
    TEMPLATE_ROAD33,
    TEMPLATE_ROAD34,
    TEMPLATE_ROAD35,
    TEMPLATE_ROAD36,
    TEMPLATE_ROAD37,
    TEMPLATE_ROAD38,
    TEMPLATE_ROAD39,
    TEMPLATE_ROAD40,
    TEMPLATE_ROAD41,
    TEMPLATE_ROAD42,
    TEMPLATE_ROAD43,

    TEMPLATE_RIVER1,
    TEMPLATE_RIVER2,
    TEMPLATE_RIVER3,
    TEMPLATE_RIVER4,
    TEMPLATE_RIVER5,
    TEMPLATE_RIVER6,
    TEMPLATE_RIVER7,
    TEMPLATE_RIVER8,
    TEMPLATE_RIVER9,
    TEMPLATE_RIVER10,
    TEMPLATE_RIVER11,
    TEMPLATE_RIVER12,
    TEMPLATE_RIVER13,

    TEMPLATE_RIVER14,
    TEMPLATE_RIVER15,
    TEMPLATE_RIVER16,
    TEMPLATE_RIVER17,
    TEMPLATE_RIVER18,
    TEMPLATE_RIVER19,
    TEMPLATE_RIVER20,
    TEMPLATE_RIVER21,
    TEMPLATE_RIVER22,
    TEMPLATE_RIVER23,
    TEMPLATE_RIVER24,
    TEMPLATE_RIVER25,
    TEMPLATE_FORD1,
    TEMPLATE_FORD2,
    TEMPLATE_FALLS1,
    TEMPLATE_FALLS2,

    TEMPLATE_BRIDGE1,
    TEMPLATE_BRIDGE1D,
    TEMPLATE_BRIDGE2,
    TEMPLATE_BRIDGE2D,
    TEMPLATE_BRIDGE3,
    TEMPLATE_BRIDGE3D,
    TEMPLATE_BRIDGE4,
    TEMPLATE_BRIDGE4D,

    TEMPLATE_SHORE24,
    TEMPLATE_SHORE25,
    TEMPLATE_SHORE26,
    TEMPLATE_SHORE27,
    TEMPLATE_SHORE28,
    TEMPLATE_SHORE29,
    TEMPLATE_SHORE30,
    TEMPLATE_SHORE31,

    TEMPLATE_PATCH16,
    TEMPLATE_PATCH17,
    TEMPLATE_PATCH18,
    TEMPLATE_PATCH19,
    TEMPLATE_PATCH20,

    TEMPLATE_SHORE34,
    TEMPLATE_SHORE35,
    TEMPLATE_SHORE36,
    TEMPLATE_SHORE37,
    TEMPLATE_SHORE38,
    TEMPLATE_SHORE39,
    TEMPLATE_SHORE40,
    TEMPLATE_SHORE41,
    TEMPLATE_SHORE42,
    TEMPLATE_SHORE43,
    TEMPLATE_SHORE44,
    TEMPLATE_SHORE45,

    TEMPLATE_SHORE46,
    TEMPLATE_SHORE47,
    TEMPLATE_SHORE48,
    TEMPLATE_SHORE49,
    TEMPLATE_SHORE50,
    TEMPLATE_SHORE51,
    TEMPLATE_SHORE52,
    TEMPLATE_SHORE53,
    TEMPLATE_SHORE54,
    TEMPLATE_SHORE55,
    TEMPLATE_SHORE56,
    TEMPLATE_SHORE57,
    TEMPLATE_SHORE58,
    TEMPLATE_SHORE59,
    TEMPLATE_SHORE60,
    TEMPLATE_SHORE61,

    TEMPLATE_SHORE62,
    TEMPLATE_SHORE63,

    TEMPLATE_COUNT,
    TEMPLATE_NONE = 255,
    TEMPLATE_FIRST = 0
} TemplateType;

inline TemplateType operator++(TemplateType& value, int) {
    int next = static_cast<int>(value) + 1;
    if (next >= static_cast<int>(TEMPLATE_COUNT)) return value;
    value = static_cast<TemplateType>(next);
    return value;
}

typedef enum OverlayType {
    OVERLAY_NONE = -1,
    OVERLAY_GENERIC = 0,
    OVERLAY_COUNT
} OverlayType;

class OverlayTypeClass {
public:
    OverlayTypeClass() : IsWall(false), IsWooden(false) {}

    static OverlayTypeClass const& As_Reference(OverlayType) {
        static OverlayTypeClass dummy;
        return dummy;
    }

    bool IsWall;
    bool IsWooden;
};

enum AnimType {
    ANIM_NONE = -1,
    ANIM_FBALL1 = 0,
    ANIM_GRENADE,
    ANIM_FRAG1,
    ANIM_FRAG2,
    ANIM_VEH_HIT1,
    ANIM_VEH_HIT2,
    ANIM_VEH_HIT3,
    ANIM_ART_EXP1,
    ANIM_NAPALM1,
    ANIM_NAPALM2,
    ANIM_NAPALM3,
    ANIM_SMOKE_PUFF,
    ANIM_PIFF,
    ANIM_PIFFPIFF,
    ANIM_FLAME_N,
    ANIM_FLAME_NE,
    ANIM_FLAME_E,
    ANIM_FLAME_SE,
    ANIM_FLAME_S,
    ANIM_FLAME_SW,
    ANIM_FLAME_W,
    ANIM_FLAME_NW,
    ANIM_CHEM_N,
    ANIM_CHEM_NE,
    ANIM_CHEM_E,
    ANIM_CHEM_SE,
    ANIM_CHEM_S,
    ANIM_CHEM_SW,
    ANIM_CHEM_W,
    ANIM_CHEM_NW,
    ANIM_FIRE_SMALL,
    ANIM_FIRE_MED,
    ANIM_FIRE_MED2,
    ANIM_FIRE_TINY,
    ANIM_MUZZLE_FLASH,
#ifdef NEVER
    ANIM_E1_ROT_FIRE,
    ANIM_E1_ROT_GRENADE,
    ANIM_E1_ROT_GUN,
    ANIM_E1_ROT_EXP,
    ANIM_E2_ROT_FIRE,
    ANIM_E2_ROT_GRENADE,
    ANIM_E2_ROT_GUN,
    ANIM_E2_ROT_EXP,
    ANIM_E3_ROT_FIRE,
    ANIM_E3_ROT_GRENADE,
    ANIM_E3_ROT_GUN,
    ANIM_E3_ROT_EXP,
    ANIM_E4_ROT_FIRE,
    ANIM_E4_ROT_GRENADE,
    ANIM_E4_ROT_GUN,
    ANIM_E4_ROT_EXP,
#endif
    ANIM_SMOKE_M,
    ANIM_BURN_SMALL,
    ANIM_BURN_MED,
    ANIM_BURN_BIG,
    ANIM_ON_FIRE_SMALL,
    ANIM_ON_FIRE_MED,
    ANIM_ON_FIRE_BIG,
    ANIM_SAM_N,
    ANIM_SAM_NE,
    ANIM_SAM_E,
    ANIM_SAM_SE,
    ANIM_SAM_S,
    ANIM_SAM_SW,
    ANIM_SAM_W,
    ANIM_SAM_NW,
    ANIM_GUN_N,
    ANIM_GUN_NE,
    ANIM_GUN_E,
    ANIM_GUN_SE,
    ANIM_GUN_S,
    ANIM_GUN_SW,
    ANIM_GUN_W,
    ANIM_GUN_NW,
    ANIM_LZ_SMOKE,
    ANIM_ION_CANNON,
    ANIM_ATOM_BLAST,
    ANIM_CRATE_DEVIATOR,
    ANIM_CRATE_DOLLAR,
    ANIM_CRATE_EARTH,
    ANIM_CRATE_EMPULSE,
    ANIM_CRATE_INVUN,
    ANIM_CRATE_MINE,
    ANIM_CRATE_RAPID,
    ANIM_CRATE_STEALTH,
    ANIM_CRATE_MISSILE,
    ANIM_ATOM_DOOR,
    ANIM_MOVE_FLASH,
    ANIM_OILFIELD_BURN,
    ANIM_TRIC_DIE,
    ANIM_TREX_DIE,
    ANIM_STEG_DIE,
    ANIM_RAPT_DIE,
    ANIM_CHEM_BALL,
    ANIM_COUNT,
    ANIM_FIRST = 0
};

inline AnimType operator++(AnimType& value, int) {
    AnimType old = value;
    value = static_cast<AnimType>(static_cast<int>(value) + 1);
    return old;
}

enum VocType {
    VOC_NONE = -1,
    VOC_RAMBO_PRESENT,
    VOC_RAMBO_CMON,
    VOC_RAMBO_UGOTIT,
    VOC_RAMBO_COMIN,
    VOC_RAMBO_LAUGH,
    VOC_RAMBO_LEFTY,
    VOC_RAMBO_NOPROB,
    VOC_RAMBO_ONIT,
    VOC_RAMBO_YELL,
    VOC_RAMBO_ROCK,
    VOC_RAMBO_TUFF,
    VOC_RAMBO_YEA,
    VOC_RAMBO_YES,
    VOC_RAMBO_YO,
    VOC_GIRL_OKAY,
    VOC_GIRL_YEAH,
    VOC_GUY_OKAY,
    VOC_GUY_YEAH,
    VOC_2DANGER,
    VOC_ACKNOWL,
    VOC_AFFIRM,
    VOC_AHA,
    VOC_ALRIGHT,
    VOC_AOK,
    VOC_APPROACH,
    VOC_AT_ONCE,
    VOC_ATTACK1,
    VOC_ATTACK2,
    VOC_BEST_SHOT,
    VOC_BLAST,
    VOC_BOOM,
    VOC_CHECK_THIS,
    VOC_CIV_ATTACK,
    VOC_CLICK,
    VOC_COME,
    VOC_COMEIN,
    VOC_CONFIRM,
    VOC_CONTACT,
    VOC_COVERME,
    VOC_COWSAY,
    VOC_DEFEND,
    VOC_DESELECT,
    VOC_DIE,
    VOC_EVILATK,
    VOC_FIRE,
    VOC_FIREREADY,
    VOC_FIRST_BLOOD,
    VOC_FOLLOWME,
    VOC_GOTIT,
    VOC_HAVEAT,
    VOC_HERE,
    VOC_HOLD,
    VOC_HOTDIG,
    VOC_HUH,
    VOC_IM_ON_IT,
    VOC_INCOMING,
    VOC_INPOSITION,
    VOC_MOVEOUT,
    VOC_NEGATIVE,
    VOC_NO_PROB,
    VOC_READY,
    VOC_REPORT,
    VOC_RIGHT_AWAY,
    VOC_ROGER,
    VOC_UGOTIT,
    VOC_UNIT,
    VOC_VEHIC,
    VOC_YESSIR,
    VOC_BAZOOKA,
    VOC_BLEEP,
    VOC_BOMB1,
    VOC_BUTTON,
    VOC_RADAR_ON,
    VOC_CONSTRUCTION,
    VOC_CRUMBLE,
    VOC_FLAMER1,
    VOC_RIFLE,
    VOC_M60,
    VOC_GUN20,
    VOC_M60A,
    VOC_MINI,
    VOC_RELOAD,
    VOC_SLAM,
    VOC_HVYGUN10,
    VOC_ION_CANNON,
    VOC_MGUN11,
    VOC_MGUN2,
    VOC_NUKE_FIRE,
    VOC_NUKE_EXPLODE,
    VOC_LASER,
    VOC_LASER_POWER,
    VOC_RADAR_OFF,
    VOC_SNIPER,
    VOC_ROCKET1,
    VOC_ROCKET2,
    VOC_MOTOR,
    VOC_SCOLD,
    VOC_SIDEBAR_OPEN,
    VOC_SIDEBAR_CLOSE,
    VOC_SQUISH2,
    VOC_TANK1,
    VOC_TANK2,
    VOC_TANK3,
    VOC_TANK4,
    VOC_UP,
    VOC_DOWN,
    VOC_TARGET,
    VOC_SONAR,
    VOC_TOSS,
    VOC_CLOAK,
    VOC_BURN,
    VOC_TURRET,
    VOC_XPLOBIG4,
    VOC_XPLOBIG6,
    VOC_XPLOBIG7,
    VOC_XPLODE,
    VOC_XPLOS,
    VOC_XPLOSML2,
    VOC_DINOATK1,
    VOC_COUNT
};

enum VoxType {
    VOX_NONE = -1,
    VOX_COUNT = 0
};

extern VoxType SpeakQueue;

struct TheaterDataType {
    char Name[16];
    char Root[10];
    char Suffix[4];
};

// Legacy text identifiers used by the type constructors.
enum TextId {
    TXT_NONE = 0,
    TXT_TRANS = 95,
    TXT_A10 = 96,
    TXT_C17 = 97,
    TXT_HELI = 108,
    TXT_ORCA = 109,

    // Building text ids referenced by bdata.cpp (values are placeholders).
    TXT_WEAPON_FACTORY,
    TXT_GUARD_TOWER,
    TXT_AGUARD_TOWER,
    TXT_OBELISK,
    TXT_TURRET,
    TXT_CONST_YARD,
    TXT_REFINERY,
    TXT_STORAGE,
    TXT_HELIPAD,
    TXT_COMMAND,
    TXT_SAM,
    TXT_AIRSTRIP,
    TXT_POWER,
    TXT_ADVANCED_POWER,
    TXT_HOSPITAL,
    TXT_BIO_LAB,
    TXT_BARRACKS,
    TXT_HAND,
    TXT_TANKER,
    TXT_FIX_IT,
    TXT_TEMPLE,
    TXT_EYE,
    TXT_MISSION,
    TXT_PUMP,
    TXT_ROAD,
    TXT_CLEAR,
    TXT_WATER,
    TXT_SLOPE,
    TXT_BRUSH,
    TXT_PATCH,
    TXT_RIVER,

    TXT_CIV1,
    TXT_CIV2,
    TXT_CIV3,
    TXT_CIV4,
    TXT_CIV5,
    TXT_CIV6,
    TXT_CIV7,
    TXT_CIV8,
    TXT_CIV9,
    TXT_CIV10,
    TXT_CIV11,
    TXT_CIV12,
    TXT_CIV13,
    TXT_CIV14,
    TXT_CIV15,
    TXT_CIV16,
    TXT_CIV17,
    TXT_CIV18,
    TXT_CIV20,
    TXT_CIV21,
    TXT_CIV22,
    TXT_CIV23,
    TXT_CIV24,
    TXT_CIV25,
    TXT_CIV26,
    TXT_CIV27,
    TXT_CIV28,
    TXT_CIV29,
    TXT_CIV30,
    TXT_CIV31,
    TXT_CIV32,
    TXT_CIV33,
    TXT_CIV34,
    TXT_CIV35,
    TXT_CIV36,
    TXT_CIV37,
    TXT_CIVMISS,

    TXT_SANDBAG_WALL,
    TXT_CYCLONE_WALL,
    TXT_BRICK_WALL,
    TXT_BARBWIRE_WALL,
    TXT_WOOD_WALL,

    TXT_PRISON,
    TXT_CIVILIAN_BUILDING,

    TXT_CONFIRMATION,
    TXT_YES,
    TXT_NO
};

// Placeholder structure for INI prerequisites.
enum StructureFlag {
    STRUCTF_NONE = 0,
    STRUCTF_POWER = 1 << 0,
    STRUCTF_REFINERY = 1 << 1,
    STRUCTF_BARRACKS = 1 << 2,
    STRUCTF_RADAR = 1 << 3,
    STRUCTF_HOSPITAL = 1 << 4,
    STRUCTF_HELIPAD = 1 << 5
};

class BuildingClass;
class TechnoClass;

// Minimal direction constants used by early ports.
static const DirType DIR_N = 0;

// Coordinate packing helpers used by data tables.
#ifndef XYP_COORD
#define XYP_COORD(x, y) (static_cast<COORDINATE>((static_cast<unsigned>(x) & 0xFFFFu) | ((static_cast<unsigned>(y) & 0xFFFFu) << 16)))
#endif

// Pixel metrics used by building data tables.
#ifndef CELL_PIXEL_W
#define CELL_PIXEL_W 24
#endif
#ifndef CELL_PIXEL_H
#define CELL_PIXEL_H 24
#endif
#ifndef ICON_PIXEL_W
#define ICON_PIXEL_W 24
#endif
#ifndef ICON_PIXEL_H
#define ICON_PIXEL_H 24
#endif

// Legacy scale used for coordinate math (see DISPLAY.H).
#ifndef ICON_LEPTON_W
#define ICON_LEPTON_W 256
#endif

#ifndef TICKS_PER_SECOND
#define TICKS_PER_SECOND 15
#endif

static const int OBELISK_ANIMATION_RATE = 3;

inline CELL Coord_Cell(COORDINATE coord) { return static_cast<CELL>(coord); }
inline int Coord_X(COORDINATE coord) { return static_cast<short>(coord & 0xFFFF); }
inline int Coord_Y(COORDINATE coord) { return static_cast<short>((coord >> 16) & 0xFFFF); }
inline CELL Coord_XCell(COORDINATE coord) { return static_cast<CELL>((coord >> 8) & 0xFF); }
inline CELL Coord_YCell(COORDINATE coord) { return static_cast<CELL>((coord >> 24) & 0xFF); }
inline int Coord_XLepton(COORDINATE coord) { return static_cast<int>(static_cast<unsigned>(coord) & 0xFFu); }
inline int Coord_YLepton(COORDINATE coord) { return static_cast<int>((static_cast<unsigned>(coord) >> 16) & 0xFFu); }

inline COORDINATE Coord_Snap(COORDINATE coord) {
    unsigned packed = static_cast<unsigned>(coord);
    packed &= 0xFF00FF00u;
    packed |= 0x00800080u;
    return static_cast<COORDINATE>(packed);
}

DirType Direction(COORDINATE coord1, COORDINATE coord2);
int Distance(COORDINATE coord1, COORDINATE coord2);
COORDINATE As_Coord(TARGET target);
BuildingClass* As_Building(TARGET target);
int Modify_Damage(int damage, WarheadType warhead, ArmorType armor, int distance);
void Explosion_Damage(COORDINATE coord, unsigned strength, TechnoClass* source, WarheadType warhead);
short const* Coord_Spillage_List(COORDINATE coord, int maxsize);
COORDINATE Coord_Move(COORDINATE start, DirType dir, unsigned short distance);
COORDINATE Coord_Scatter(COORDINATE coord, unsigned distance, bool lock);
void Move_Point(short& x, short& y, DirType dir, unsigned short distance);

inline COORDINATE Cell_Coord(CELL cell) { return static_cast<COORDINATE>(cell); }

class AbstractClass {
public:
    AbstractClass() : Coord(0), IsActive(0) {}
    virtual ~AbstractClass() {}

    virtual RTTIType What_Am_I(void) const { return RTTI_NONE; }
    virtual HousesType Owner(void) const { return HOUSE_NONE; }

    virtual COORDINATE Center_Coord(void) const { return Coord; }
    virtual COORDINATE Target_Coord(void) const { return Coord; }

    DirType Direction(AbstractClass const* object) const {
        return ::Direction(Center_Coord(), object ? object->Target_Coord() : Center_Coord());
    }
    DirType Direction(TARGET target) const { return ::Direction(Center_Coord(), ::As_Coord(target)); }
    int Distance(TARGET target) const;
    int Distance(AbstractClass const* object) const {
        return ::Distance(Center_Coord(), object ? object->Target_Coord() : Center_Coord());
    }

    virtual MoveType Can_Enter_Cell(CELL, FacingType = FACING_NONE) const { return MOVE_OK; }

    COORDINATE Coord;
    unsigned IsActive : 1;
};

class AbstractTypeClass {
public:
    AbstractTypeClass() : IniName(), Name(0) {}
    AbstractTypeClass(int name, char const* ini);
    virtual ~AbstractTypeClass() {}

    virtual RTTIType What_Am_I(void) const;
    virtual COORDINATE Coord_Fixup(COORDINATE coord) const;
    virtual int Full_Name(void) const;
    void Set_Name(char const* buf) const {
        std::strncpy((char*)IniName, buf, sizeof(IniName));
        ((char&)IniName[sizeof(IniName) - 1]) = '\0';
    }
    virtual unsigned short Get_Ownable(void) const;

    char IniName[9];
    int Name;
};

class TechnoClass;

class ObjectClass : public AbstractClass {
public:
    ObjectClass() : Next(NULL), IsDown(true), IsInLimbo(false), IsToDamage(false) {}
    virtual ~ObjectClass() {}

    virtual ResultType Take_Damage(int& damage, int distance, WarheadType warhead, TechnoClass* source = 0) {
        (void)damage;
        (void)distance;
        (void)warhead;
        (void)source;
        return RESULT_NONE;
    }

    ObjectClass* Next;
    bool IsDown;
    bool IsInLimbo;
    bool IsToDamage;
};

class FootClass : public ObjectClass {
public:
    virtual ~FootClass() {}

    virtual void Limbo() {}
    virtual TARGET As_Target() const { return 0; }
};

class TechnoClass : public ObjectClass {
public:
    virtual ~TechnoClass() {}
};

class HouseTypeClass {
public:
    explicit HouseTypeClass(HousesType house = HOUSE_NONE) : House(house) {}
    HousesType House;

    static HousesType From_Name(char const* name) {
        if (!name) return HOUSE_NONE;
        if (!strcasecmp(name, "GoodGuy")) return HOUSE_GOOD;
        if (!strcasecmp(name, "BadGuy")) return HOUSE_BAD;
        if (!strcasecmp(name, "Neutral")) return HOUSE_NEUTRAL;
        if (!strcasecmp(name, "Japan")) return HOUSE_JP;
        return HOUSE_NONE;
    }
};

class HouseClass {
public:
    struct TypeRef : public HouseTypeClass {
        explicit TypeRef(HousesType house = HOUSE_NONE) : HouseTypeClass(house) {}

        HouseTypeClass* operator->() { return this; }
        HouseTypeClass const* operator->() const { return this; }
    };

    explicit HouseClass(HousesType house = HOUSE_NONE) : Class(house) {}

    bool Can_Build(AircraftType, int) const { return true; }
    bool Can_Build(StructType, int) const { return true; }
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

    TypeRef Class;
};

class BuildingTypeClass;

class BuildingClass : public ObjectClass {
public:
    BuildingClass();
    BuildingClass(StructType type, HousesType owner);

    bool Unlimbo(COORDINATE, DirType) { return true; }
    bool In_Radio_Contact() const { return false; }

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

class FileClass {
public:
    explicit FileClass(const char* filename);
    virtual ~FileClass();

    virtual bool Is_Available();
    virtual long Size();
    virtual long Read(void* buffer, long length);
    virtual long Write(void const* buffer, long length);
    virtual void Close();

protected:
    std::string Filename;
    std::FILE* Handle;
};

class RawFileClass : public FileClass {
public:
    explicit RawFileClass(const char* filename) : FileClass(filename) {}
    RawFileClass() : FileClass(NULL) {}

    virtual char const* Set_Name(char const* filename) {
        Filename = filename ? filename : "";
        Close();
        return Filename.empty() ? NULL : Filename.c_str();
    }

    char const* File_Name() const { return Filename.empty() ? NULL : Filename.c_str(); }

    virtual int Open(int rights = READ) {
        if (rights & WRITE) {
            Close();
            if (Filename.empty()) return false;
            Handle = std::fopen(Filename.c_str(), "wb");
            return Handle != NULL;
        }
        return FileClass::Is_Available() ? true : false;
    }

    virtual int Open(char const* filename, int rights = READ) {
        Set_Name(filename);
        return Open(rights);
    }

    virtual int Is_Open() const { return Handle != NULL; }

    virtual int Is_Available(int forced = false) {
        (void)forced;
        return FileClass::Is_Available() ? true : false;
    }

    virtual long Seek(long pos, int dir = SEEK_CUR) {
        if (!Is_Open() && !Open(READ)) return 0;
        if (!Handle) return 0;
        if (std::fseek(Handle, pos, dir) != 0) return 0;
        return std::ftell(Handle);
    }
};

int WWGetPrivateProfileInt(char const* section, char const* key, int default_value, char const* buffer);
int WWGetPrivateProfileString(char const* section,
                              char const* key,
                              char const* default_value,
                              char* output,
                              int output_len,
                              char const* buffer);
bool WWWritePrivateProfileString(char const* section, char const* key, char const* value, char* buffer);
bool WWWritePrivateProfileInt(char const* section, char const* key, int value, char* buffer);

class MapCellClass {
public:
    // Placeholder retained for historical reasons; superseded by CellClass-backed MapClass.
};

class CellClass;

class MapClass {
public:
    MapClass();
    ~MapClass();

    CellClass& operator[](CELL cell);
    CellClass const& operator[](CELL cell) const;
    bool In_Radar(CELL) const { return true; }

private:
    CellClass* Dummy;
};

extern MapClass Map;

class ObjectTypeClass : public AbstractTypeClass {
public:
    ObjectTypeClass(bool is_sentient,
                    bool is_flammable,
                    bool is_crushable,
                    bool is_stealthy,
                    bool is_selectable,
                    bool is_legal_target,
                    bool is_insignificant,
                    bool is_immune,
                    int fullname,
                    char const* name,
                    ArmorType armor,
                    unsigned short strength)
        : AbstractTypeClass(fullname, name),
          IsCrushable(is_crushable),
          IsStealthy(is_stealthy),
          IsSelectable(is_selectable),
          IsLegalTarget(is_legal_target),
          IsInsignificant(is_insignificant),
          IsImmune(is_immune),
          IsFlammable(is_flammable),
          IsSentient(is_sentient),
          Armor(armor),
          MaxStrength(strength),
          ImageData(NULL),
          RadarIcon(NULL) {}

    virtual ~ObjectTypeClass() {}

    void const* Get_Image_Data(void) const { return ImageData; }

    bool IsCrushable;
    bool IsStealthy;
    bool IsSelectable;
    bool IsLegalTarget;
    bool IsInsignificant;
    bool IsImmune;
    bool IsFlammable;
    bool IsSentient;
    ArmorType Armor;
    unsigned short MaxStrength;
    void const* ImageData;
    void const* RadarIcon;

    static void const* SelectShapes;
    static void const* PipShapes;
};

class TemplateClass : public ObjectClass {
public:
    TemplateClass() : Type(TEMPLATE_NONE), Cell(-1) {}
    TemplateClass(TemplateType type, CELL cell = -1) : Type(type), Cell(cell) {}

    TemplateType Type;
    CELL Cell;
};

class AnimTypeClass : public ObjectTypeClass {
public:
    AnimTypeClass(AnimType anim,
                  char const* name,
                  int size,
                  int biggest,
                  bool isnormal,
                  bool iswhite,
                  bool isscorcher,
                  bool iscrater,
                  bool issticky,
                  bool ground,
                  bool istrans,
                  bool isflame,
                  unsigned int damage,
                  int delaytime,
                  int start,
                  int loopstart,
                  int loopend,
                  int stages,
                  int loops,
                  VocType sound,
                  AnimType chainto);

    virtual RTTIType What_Am_I(void) const { return RTTI_ANIMTYPE; }

    static AnimTypeClass const& As_Reference(AnimType type) { return *Pointers[type]; }
    static void Init(TheaterType) {}
    static void One_Time(void);

    AnimType Type;
    int Size;
    int Biggest;
    unsigned int Damage;
    unsigned char Delay;
    int Start;
    int LoopStart;
    int LoopEnd;
    int Stages;
    unsigned char Loops;
    VocType Sound;
    AnimType ChainTo;
    bool IsNormalized;
    bool IsGroundLayer;
    bool IsTranslucent;
    bool IsWhiteTrans;
    bool IsFlameThrower;
    bool IsScorcher;
    bool IsCraterForming;
    bool IsSticky;

private:
    static AnimTypeClass const* const Pointers[ANIM_COUNT];
};

class BulletTypeClass : public ObjectTypeClass {
public:
    BulletTypeClass(BulletType type,
                    char const* ininame,
                    bool is_high,
                    bool is_homing,
                    bool is_arcing,
                    bool is_dropping,
                    bool is_invisible,
                    bool is_proximity_armed,
                    bool is_flame_equipped,
                    bool is_fueled,
                    bool is_faceless,
                    bool is_inaccurate,
                    bool is_translucent,
                    bool is_antiaircraft,
                    int arming,
                    int range,
                    MPHType maxspeed,
                    unsigned rot,
                    WarheadType warhead,
                    AnimType explosion);

    static BulletTypeClass const& As_Reference(BulletType type) { return *Pointers[static_cast<int>(type)]; }
    static void One_Time(void);

    BulletType Type;
    WarheadType Warhead;
    AnimType Explosion;

    MPHType MaxSpeed;
    unsigned ROT;
    int Arming;
    int Range;

    unsigned IsHigh : 1;
    unsigned IsHoming : 1;
    unsigned IsArcing : 1;
    unsigned IsDropping : 1;
    unsigned IsInvisible : 1;
    unsigned IsProximityArmed : 1;
    unsigned IsFlameEquipped : 1;
    unsigned IsFueled : 1;
    unsigned IsFaceless : 1;
    unsigned IsInaccurate : 1;
    unsigned IsTranslucent : 1;
    unsigned IsAntiAircraft : 1;

private:
    static BulletTypeClass const* const Pointers[BULLET_COUNT];
};

class TechnoTypeClass : public AbstractTypeClass {
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
        : AbstractTypeClass(name, ininame),
          CameoData(NULL),
          ImageData(NULL),
          MaxStrength(strength),
          Primary(primary),
          Secondary(secondary),
          Cost(cost),
          IsNominal(is_nominal),
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
        (void)armor;
    }

    virtual ~TechnoTypeClass() {}

    virtual int Max_Passengers() const { return 0; }
    virtual int Full_Name(void) const { return AbstractTypeClass::Full_Name(); }
    virtual int Raw_Cost(void) const { return Cost; }
    virtual int Cost_Of(void) const { return Cost; }

    void const* CameoData;
    void const* ImageData;
    unsigned short MaxStrength;
    WeaponType Primary;
    WeaponType Secondary;
    int Cost;
    bool IsNominal;
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
template <typename T>
inline T Bound(T value, T low, T high) {
    if (value < low) return low;
    if (value > high) return high;
    return value;
}

inline int Bound(unsigned value, int low, int high) {
    return Bound(static_cast<int>(value), low, high);
}

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

static const int REPAIR_PERCENT = 102;
static const int REPAIR_STEP = 2;

struct SpecialClass {
    bool IsRoad;
    bool IsNamed;
    bool IsSeparate;
    bool IsInert;
    SpecialClass() : IsRoad(false), IsNamed(false), IsSeparate(false), IsInert(false) {}
};

extern SpecialClass Special;
extern int Scenario;
extern bool Debug_Map;

int Get_Build_Frame_Count(void const*);

class UnitTypeClass {
public:
    explicit UnitTypeClass(int cost = 0) : Cost(cost) {}
    int Cost;
    static UnitTypeClass const& As_Reference(int) {
        static UnitTypeClass dummy(0);
        return dummy;
    }
};

static const int UNIT_HARVESTER = 0;

#include "buildingtype.h"

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

// Compatibility helpers.
inline int stricmp(const char* a, const char* b) { return ::strcasecmp(a, b); }

inline void _makepath(char* path, const char*, const char* dir, const char* fname, const char* ext) {
    if (!path) return;
    const char* directory = dir ? dir : "";
    const char* filename = fname ? fname : "";
    const char* extension = ext ? ext : "";
    std::snprintf(path, _MAX_FNAME + _MAX_EXT, "%s%s%s", directory, filename, extension);
}

inline void Mem_Copy(void const* source, void* dest, int bytes) {
    if (!dest || bytes <= 0) return;
    if (!source) {
        std::memset(dest, 0, static_cast<size_t>(bytes));
        return;
    }
    std::memcpy(dest, source, static_cast<size_t>(bytes));
}

inline unsigned char const* Get_Icon_Set_Map(void const*) {
    static unsigned char empty_map[13 * 8];
    static bool initialized = false;
    if (!initialized) {
        std::memset(empty_map, 0xFF, sizeof(empty_map));
        initialized = true;
    }
    return empty_map;
}

inline void Register_Icon_Set(void*, bool) {}

int Fixed_To_Cardinal(int value, int percent_fixed);
int Get_Resolution_Factor(void);
void* Load_Alloc_Data(FileClass& file);
void CCDebugString(char const* string);

namespace MixFileClass {
void const* Retrieve(const char* filename);
bool Offset(char const* filename,
            void** realptr = 0,
            char const** mix_filename = 0,
            long* offset = 0,
            long* size = 0);
}

extern BuildingCollection Buildings;
extern TheaterDataType const Theaters[THEATER_COUNT];
extern TheaterType LastTheater;

// Most ported sources include only this header; include commonly-needed core types here.
#include "cell.h"
