#include "legacy/map.h"
#include "legacy/logic.h"
#include "legacy/utracker.h"

int const MapClass::RadiusCount[11] = {};
int const MapClass::RadiusOffset[] = {0};

void MapClass::One_Time(void) {}

void MapClass::Init_Clear(void) {}

void MapClass::Alloc_Cells(void) {}

void MapClass::Free_Cells(void) {}

void MapClass::Init_Cells(void) {}

ObjectClass* MapClass::Close_Object(COORDINATE) const { return nullptr; }

int MapClass::Cell_Region(CELL) { return 0; }

int MapClass::Cell_Threat(CELL, HousesType) { return 0; }

int MapClass::Cell_Distance(CELL, CELL) { return 0; }

bool MapClass::In_Radar(CELL) const { return false; }

void MapClass::Sight_From(CELL, int, bool) {}

void MapClass::Place_Down(CELL, ObjectClass*) {}

void MapClass::Pick_Up(CELL, ObjectClass*) {}

void MapClass::Overlap_Down(CELL, ObjectClass*) {}

void MapClass::Overlap_Up(CELL, ObjectClass*) {}

bool MapClass::Read_Binary(char const*, unsigned long*) { return false; }

bool MapClass::Write_Binary(char const*) { return false; }

bool MapClass::Place_Random_Crate(void) { return false; }

long MapClass::Overpass(void) { return 0; }

void MapClass::Logic(void) {}

void MapClass::Set_Map_Dimensions(int, int, int, int) {}

int MapClass::Validate(void) { return 0; }

void LogicClass::AI(void) {}

void LogicClass::Debug_Dump(MonoClass*) const {}

// UnitTrackerClass is implemented in utracker.cpp; do not provide
// duplicate definitions here to avoid linker conflicts.
