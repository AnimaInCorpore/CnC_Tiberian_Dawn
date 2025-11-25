#include "legacy/function.h"
#include "legacy/externs.h"

/*
**  Ported constants from the original CONST.CPP. These tables describe
**  facing lookup helpers, weapon/warhead definitions, and various
**  coordinate utilities used throughout the game logic.
*/

char const* const SourceName[SOURCE_COUNT] = {
    "North",    "East",  "South",   "West",    "Shipping",
    "Beach",    "Air",   "Visible", "EnemyBase", "HomeBase",
    "Ocean",
};

COORDINATE const StoppingCoordAbs[5] = {
    0x00800080L,  // center
    0x00400040L,  // upper left
    0x004000C0L,  // upper right
    0x00C00040L,  // lower left
    0x00C000C0L   // lower right
};

WeaponTypeClass const Weapons[WEAPON_COUNT] = {
    {BULLET_SNIPER, 125, 40, 0x0580, VOC_SNIPER, ANIM_NONE},       // WEAPON_RIFLE
    {BULLET_SPREADFIRE, 25, 50, 0x0400, VOC_MINI, ANIM_GUN_N},     // WEAPON_CHAIN_GUN
    {BULLET_BULLET, 1, 7, 0x01C0, VOC_RIFLE, ANIM_NONE},           // WEAPON_PISTOL
    {BULLET_BULLET, 15, 20, 0x0200, VOC_MGUN2, ANIM_NONE},         // WEAPON_M16
    {BULLET_TOW, 30, 60, 0x0400, VOC_BAZOOKA, ANIM_NONE},          // WEAPON_DRAGON
    {BULLET_FLAME, 35, 50, 0x0200, VOC_FLAMER1, ANIM_FLAME_N},     // WEAPON_FLAMETHROWER
    {BULLET_FLAME, 50, 50, 0x0200, VOC_FLAMER1, ANIM_FLAME_N},     // WEAPON_FLAME_TONGUE
    {BULLET_CHEMSPRAY, 80, 70, 0x0200, VOC_FLAMER1, ANIM_CHEM_N},  // WEAPON_CHEMSPRAY
    {BULLET_GRENADE, 50, 60, 0x0340, VOC_TOSS, ANIM_NONE},         // WEAPON_GRENADE
    {BULLET_APDS, 25, 60, 0x0400, VOC_TANK2, ANIM_MUZZLE_FLASH},   // WEAPON_75MM
    {BULLET_APDS, 30, 50, 0x04C0, VOC_TANK3, ANIM_MUZZLE_FLASH},   // WEAPON_105MM
    {BULLET_APDS, 40, 80, 0x04C0, VOC_TANK4, ANIM_MUZZLE_FLASH},   // WEAPON_120MM
    {BULLET_APDS, 40, 60, 0x0600, VOC_TANK4, ANIM_MUZZLE_FLASH},   // WEAPON_TURRET_GUN
    {BULLET_SSM, 75, 80, 0x0500, VOC_ROCKET1, ANIM_NONE},          // WEAPON_MAMMOTH_TUSK
    {BULLET_SSM2, 75, 80, 0x0600, VOC_ROCKET1, ANIM_NONE},         // WEAPON_MLRS
    {BULLET_HE, 150, 65, 0x0600, VOC_TANK1, ANIM_MUZZLE_FLASH},    // WEAPON_155MM
    {BULLET_BULLET, 15, 30, 0x0400, VOC_MGUN11, ANIM_GUN_N},       // WEAPON_M60MG
    {BULLET_SSM, 60, 35, 0x0780, VOC_ROCKET2, ANIM_NONE},          // WEAPON_TOMAHAWK
    {BULLET_SSM, 60, 40, 0x0680, VOC_ROCKET2, ANIM_NONE},          // WEAPON_TOW_TWO
    {BULLET_NAPALM, 100, 20, 0x0480, VOC_NONE, ANIM_NONE},         // WEAPON_NAPALM
    {BULLET_LASER, 200, 90, 0x0780, VOC_LASER, ANIM_NONE},         // WEAPON_OBELISK_LASER
    {BULLET_SAM, 50, 50, 0x0780, VOC_ROCKET2, ANIM_NONE},          // WEAPON_NIKE
    {BULLET_HONEST_JOHN, 100, 200, 0x0A00, VOC_ROCKET1, ANIM_NONE},  // WEAPON_HONEST_JOHN
    {BULLET_HEADBUTT, 100, 30, 0x0180, VOC_DINOATK1, ANIM_NONE},     // WEAPON_STEG
    {BULLET_TREXBITE, 155, 30, 0x0180, VOC_DINOATK1, ANIM_NONE},     // WEAPON_TREX
};

WarheadTypeClass const Warheads[WARHEAD_COUNT] = {
    {2, false, false, false, {0xFF, 0x80, 0x90, 0x40, 0x40}},   // WARHEAD_SA
    {6, true, true, true, {0xE0, 0xC0, 0x90, 0x40, 0xFF}},      // WARHEAD_HE
    {6, true, true, false, {0x40, 0xC0, 0xC0, 0xFF, 0x80}},     // WARHEAD_AP
    {8, false, true, true, {0xE0, 0xFF, 0xB0, 0x40, 0x80}},     // WARHEAD_FIRE
    {4, false, false, false, {0xFF, 0xFF, 0xFF, 0xFF, 0xFF}},   // WARHEAD_LASER
    {7, true, true, true, {0xFF, 0xFF, 0xC0, 0xC0, 0xC0}},      // WARHEAD_PB
    {4, false, false, false, {0xFF, 0x20, 0x20, 0x10, 0x10}},   // WARHEAD_FIST
    {4, false, false, false, {0xFF, 0x20, 0x20, 0x10, 0x10}},   // WARHEAD_FOOT
    {4, false, false, false, {0xFF, 0x08, 0x08, 0x08, 0x08}},   // WARHEAD_HOLLOW_POINT
    {255, false, false, false, {0xFF, 0x01, 0x01, 0x01, 0x01}},  // WARHEAD_SPORE
    {1, true, true, false, {0xFF, 0xC0, 0x80, 0x20, 0x08}},     // WARHEAD_HEADBUTT
    {1, true, true, false, {0xFF, 0xC0, 0x80, 0x20, 0x08}},     // WARHEAD_FEEDME
};

unsigned char const Pixel2Lepton[24] = {
    0x00, 0x0B, 0x15, 0x20, 0x2B, 0x35, 0x40, 0x4B,
    0x55, 0x60, 0x6B, 0x75, 0x80, 0x8B, 0x95, 0xA0,
    0xAB, 0xB5, 0xC0, 0xCB, 0xD5, 0xE0, 0xEB, 0xF5};

CELL const AdjacentCell[FACING_COUNT] = {
    -(MAP_CELL_W),     // North
    -(MAP_CELL_W - 1), // North East
    1,                 // East
    MAP_CELL_W + 1,    // South East
    MAP_CELL_W,        // South
    MAP_CELL_W - 1,    // South West
    -1,                // West
    -(MAP_CELL_W + 1)  // North West
};

COORDINATE const AdjacentCoord[FACING_COUNT] = {
    0xFF000000L, 0xFF000100L, 0x00000100L, 0x01000100L,
    0x01000000L, 0x0100FF00L, 0x0000FF00L, 0xFF00FF00L};

unsigned char const Facing8[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0};

unsigned char const Facing32[256] = {
    0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 6, 6,
    6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9,
    9, 9, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12,
    12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 19, 19, 19,
    19, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 23,
    23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 26, 26,
    26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 28, 28, 29, 29,
    29, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 31, 31,
    31, 31, 0, 0, 0, 0, 0, 0};

TheaterDataType const Theaters[THEATER_COUNT] = {
    {"DESERT", "DESERT", "DES"},
    {"JUNGLE", "JUNGLE", "JUN"},
    {"TEMPERATE", "TEMPERAT", "TEM"},
    {"WINTER", "WINTER", "WIN"},
};
