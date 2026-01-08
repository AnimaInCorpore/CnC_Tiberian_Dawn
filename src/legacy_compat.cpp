#include "legacy_compat.h"

BuildingCollection Buildings;

TheaterDataType Theaters[THEATER_COUNT] = {
    {"DESERT", "DESERT", "DES"},
    {"JUNGLE", "JUNGLE", "JUN"},
    {"TEMPERATE", "TEMPERAT", "TEM"},
    {"WINTER", "WINTER", "WIN"},
};

TheaterType LastTheater = THEATER_NONE;

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
