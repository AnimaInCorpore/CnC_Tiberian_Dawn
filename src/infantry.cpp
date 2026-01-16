/*
 * Portable build stub for legacy INFANTRY.CPP.
 *
 * The full infantry implementation is one of the most interconnected modules
 * in the original game (type tables, map interaction, missions, weapons,
 * animations, radio, and UI click handling). Porting it requires the Techno
 * stack and type system to be in place first.
 *
 * Keep this translation unit present so the CMake + SDL 1.2 build remains
 * stable while related modules are migrated.
 */

#include "infantry.h"

InfantryClass::InfantryClass() {}

InfantryClass::~InfantryClass() {}

RTTIType InfantryClass::What_Am_I(void) const { return RTTI_INFANTRY; }

TARGET InfantryClass::As_Target(void) const { return TARGET_NONE; }

