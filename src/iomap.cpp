#include "map.h"

void MapClass::Code_Pointers(void)
{
    for (CELL cell = 0; cell < MAP_CELL_TOTAL; ++cell) {
        (*this)[cell].Code_Pointers();
    }

    GScreenClass::Code_Pointers();
}

void MapClass::Decode_Pointers(void)
{
    for (CELL cell = 0; cell < MAP_CELL_TOTAL; ++cell) {
        (*this)[cell].Decode_Pointers();
    }

    GScreenClass::Decode_Pointers();
}

