#include "legacy_compat.h"

#include <cstdlib>

BuildingCollection Buildings;

TheaterDataType Theaters[THEATER_COUNT] = {
    {"DESERT", "DESERT", "DES"},
    {"JUNGLE", "JUNGLE", "JUN"},
    {"TEMPERATE", "TEMPERAT", "TEM"},
    {"WINTER", "WINTER", "WIN"},
};

TheaterType LastTheater = THEATER_NONE;

COORDINATE As_Coord(TARGET target) { return static_cast<COORDINATE>(target); }

BuildingClass* As_Building(TARGET) { return NULL; }

DirType Direction(COORDINATE coord1, COORDINATE coord2) {
    (void)coord1;
    (void)coord2;
    return FACING_NONE;
}

int Distance(COORDINATE coord1, COORDINATE coord2) { return std::abs(coord2 - coord1); }

int Fixed_To_Cardinal(int value, int percent_fixed) {
    return (value * percent_fixed) / 256;
}

int Get_Resolution_Factor(void) {
    return 0;
}

namespace MixFileClass {
void const* Retrieve(const char* filename) {
    (void)filename;
    return NULL;
}
}  // namespace MixFileClass
