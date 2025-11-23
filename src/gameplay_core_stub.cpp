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

void MapClass::Detach(ObjectClass*) {}

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

bool MapClass::Write_Binary(char const*, unsigned long*) { return false; }

bool MapClass::Place_Random_Crate(void) { return false; }

long MapClass::Overpass(void) { return 0; }

void MapClass::Logic(void) {}

void MapClass::Set_Map_Dimensions(int, int, int, int) {}

void MapClass::Code_Pointers(void) {}

void MapClass::Decode_Pointers(void) {}

int MapClass::Validate(void) { return 0; }

void LogicClass::AI(void) {}

UnitTrackerClass::UnitTrackerClass(int unit_count)
    : UnitTotals(nullptr), UnitCount(unit_count), InNetworkFormat(0) {}

UnitTrackerClass::~UnitTrackerClass(void) { delete[] UnitTotals; }

void UnitTrackerClass::Increment_Unit_Total(int) {}

void UnitTrackerClass::Decrement_Unit_Total(int) {}

void UnitTrackerClass::Clear_Unit_Total(void) {}

int UnitTrackerClass::Get_Unit_Total(int) { return 0; }

long* UnitTrackerClass::Get_All_Totals(void) { return UnitTotals; }

void UnitTrackerClass::To_Network_Format(void) { InNetworkFormat = 1; }

void UnitTrackerClass::To_PC_Format(void) { InNetworkFormat = 0; }
