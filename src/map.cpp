/*
**  MapClass implementation
*/

#include "map.h"
#include "legacy_compat.h"
#include <cstring>
#include <cstdlib>

// CellTriggers is a global parallel array to Map.
// We declare it extern here (defined in globals.cpp or legacy_compat.cpp shim).
// If not defined elsewhere, we might need a shim, but grep showed it in globals.cpp.
extern DynamicVectorClass<TriggerClass*> CellTriggers;

#define MCW MAP_CELL_W

// Radius lookup tables
int const MapClass::RadiusOffset[] = {
    /* 0  */ 0,
    /* 1  */ (-MCW * 1) - 1, (-MCW * 1) + 0, (-MCW * 1) + 1, -1, 1, (MCW * 1) - 1, (MCW * 1) + 0, (MCW * 1) + 1,
    /* 2  */ (-MCW * 2) - 1, (-MCW * 2) + 0, (-MCW * 2) + 1, (-MCW * 1) - 2, (-MCW * 1) + 2, -2, 2, (MCW * 1) - 2, (MCW * 1) + 2, (MCW * 2) - 1, (MCW * 2) + 0, (MCW * 2) + 1,
    /* 3  */ (-MCW * 3) - 1, (-MCW * 3) + 0, (-MCW * 3) + 1, (-MCW * 2) - 2, (-MCW * 2) + 2, (-MCW * 1) - 3, (-MCW * 1) + 3, -3, 3, (MCW * 1) - 3, (MCW * 1) + 3, (MCW * 2) - 2, (MCW * 2) + 2, (MCW * 3) - 1, (MCW * 3) + 0, (MCW * 3) + 1,
    /* 4  */ (-MCW * 4) - 1, (-MCW * 4) + 0, (-MCW * 4) + 1, (-MCW * 3) - 3, (-MCW * 3) - 2, (-MCW * 3) + 2, (-MCW * 3) + 3, (-MCW * 2) - 3, (-MCW * 2) + 3, (-MCW * 1) - 4, (-MCW * 1) + 4, -4, 4, (MCW * 1) - 4, (MCW * 1) + 4, (MCW * 2) - 3, (MCW * 2) + 3, (MCW * 3) - 3, (MCW * 3) - 2, (MCW * 3) + 2, (MCW * 3) + 3, (MCW * 4) - 1, (MCW * 4) + 0, (MCW * 4) + 1,
    /* 5  */ (-MCW * 5) - 1, (-MCW * 5) + 0, (-MCW * 5) + 1, (-MCW * 4) - 3, (-MCW * 4) - 2, (-MCW * 4) + 2, (-MCW * 4) + 3, (-MCW * 3) - 4, (-MCW * 3) + 4, (-MCW * 2) - 4, (-MCW * 2) + 4, (-MCW * 1) - 5, (-MCW * 1) + 5, -5, 5, (MCW * 1) - 5, (MCW * 1) + 5, (MCW * 2) - 4, (MCW * 2) + 4, (MCW * 3) - 4, (MCW * 3) + 4, (MCW * 4) - 3, (MCW * 4) - 2, (MCW * 4) + 2, (MCW * 4) + 3, (MCW * 5) - 1, (MCW * 5) + 0, (MCW * 5) + 1,
    /* 6  */ (-MCW * 6) - 1, (-MCW * 6) + 0, (-MCW * 6) + 1, (-MCW * 5) - 3, (-MCW * 5) - 2, (-MCW * 5) + 2, (-MCW * 5) + 3, (-MCW * 4) - 4, (-MCW * 4) + 4, (-MCW * 3) - 5, (-MCW * 3) + 5, (-MCW * 2) - 5, (-MCW * 2) + 5, (-MCW * 1) - 6, (-MCW * 1) + 6, -6, 6, (MCW * 1) - 6, (MCW * 1) + 6, (MCW * 2) - 5, (MCW * 2) + 5, (MCW * 3) - 5, (MCW * 3) + 5, (MCW * 4) - 4, (MCW * 4) + 4, (MCW * 5) - 3, (MCW * 5) - 2, (MCW * 5) + 2, (MCW * 5) + 3, (MCW * 6) - 1, (MCW * 6) + 0, (MCW * 6) + 1,
    /* 7  */ (-MCW * 7) - 1, (-MCW * 7) + 0, (-MCW * 7) + 1, (-MCW * 6) - 3, (-MCW * 6) - 2, (-MCW * 6) + 2, (-MCW * 6) + 3, (-MCW * 5) - 5, (-MCW * 5) - 4, (-MCW * 5) + 4, (-MCW * 5) + 5, (-MCW * 4) - 5, (-MCW * 4) + 5, (-MCW * 3) - 6, (-MCW * 3) + 6, (-MCW * 2) - 6, (-MCW * 2) + 6, (-MCW * 1) - 7, (-MCW * 1) + 7, -7, 7, (MCW * 1) - 7, (MCW * 1) + 7, (MCW * 2) - 6, (MCW * 2) + 6, (MCW * 3) - 6, (MCW * 3) + 6, (MCW * 4) - 5, (MCW * 4) + 5, (MCW * 5) - 5, (MCW * 5) - 4, (MCW * 5) + 4, (MCW * 5) + 5, (MCW * 6) - 3, (MCW * 6) - 2, (MCW * 6) + 2, (MCW * 6) + 3, (MCW * 7) - 1, (MCW * 7) + 0, (MCW * 7) + 1,
    /* 8  */ (-MCW * 8) - 1, (-MCW * 8) + 0, (-MCW * 8) + 1, (-MCW * 7) - 3, (-MCW * 7) - 2, (-MCW * 7) + 2, (-MCW * 7) + 3, (-MCW * 6) - 5, (-MCW * 6) - 4, (-MCW * 6) + 4, (-MCW * 6) + 5, (-MCW * 5) - 6, (-MCW * 5) + 6, (-MCW * 4) - 6, (-MCW * 4) + 6, (-MCW * 3) - 7, (-MCW * 3) + 7, (-MCW * 2) - 7, (-MCW * 2) + 7, (-MCW * 1) - 8, (-MCW * 1) + 8, -8, 8, (MCW * 1) - 8, (MCW * 1) + 8, (MCW * 2) - 7, (MCW * 2) + 7, (MCW * 3) - 7, (MCW * 3) + 7, (MCW * 4) - 6, (MCW * 4) + 6, (MCW * 5) - 6, (MCW * 5) + 6, (MCW * 6) - 5, (MCW * 6) - 4, (MCW * 6) + 4, (MCW * 6) + 5, (MCW * 7) - 3, (MCW * 7) - 2, (MCW * 7) + 2, (MCW * 7) + 3, (MCW * 8) - 1, (MCW * 8) + 0, (MCW * 8) + 1,
    /* 9  */ (-MCW * 9) - 1, (-MCW * 9) + 0, (-MCW * 9) + 1, (-MCW * 8) - 3, (-MCW * 8) - 2, (-MCW * 8) + 2, (-MCW * 8) + 3, (-MCW * 7) - 5, (-MCW * 7) - 4, (-MCW * 7) + 4, (-MCW * 7) + 5, (-MCW * 6) - 6, (-MCW * 6) + 6, (-MCW * 5) - 7, (-MCW * 5) + 7, (-MCW * 4) - 7, (-MCW * 4) + 7, (-MCW * 3) - 8, (-MCW * 3) + 8, (-MCW * 2) - 8, (-MCW * 2) + 8, (-MCW * 1) - 9, (-MCW * 1) + 9, -9, 9, (MCW * 1) - 9, (MCW * 1) + 9, (MCW * 2) - 8, (MCW * 2) + 8, (MCW * 3) - 8, (MCW * 3) + 8, (MCW * 4) - 7, (MCW * 4) + 7, (MCW * 5) - 7, (MCW * 5) + 7, (MCW * 6) - 6, (MCW * 6) + 6, (MCW * 7) - 5, (MCW * 7) - 4, (MCW * 7) + 4, (MCW * 7) + 5, (MCW * 8) - 3, (MCW * 8) - 2, (MCW * 8) + 2, (MCW * 8) + 3, (MCW * 9) - 1, (MCW * 9) + 0, (MCW * 9) + 1,
    /* 10 */ (-MCW * 10) - 1, (-MCW * 10) + 0, (-MCW * 10) + 1, (-MCW * 9) - 3, (-MCW * 9) - 2, (-MCW * 9) + 2, (-MCW * 9) + 3, (-MCW * 8) - 5, (-MCW * 8) - 4, (-MCW * 8) + 4, (-MCW * 8) + 5, (-MCW * 7) - 7, (-MCW * 7) - 6, (-MCW * 7) + 6, (-MCW * 7) + 7, (-MCW * 6) - 7, (-MCW * 6) + 7, (-MCW * 5) - 8, (-MCW * 5) + 8, (-MCW * 4) - 8, (-MCW * 4) + 8, (-MCW * 3) - 9, (-MCW * 3) + 9, (-MCW * 2) - 9, (-MCW * 2) + 9, (-MCW * 1) - 10, (-MCW * 1) + 10, -10, 10, (MCW * 1) - 10, (MCW * 1) + 10, (MCW * 2) - 9, (MCW * 2) + 9, (MCW * 3) - 9, (MCW * 3) + 9, (MCW * 4) - 8, (MCW * 4) + 8, (MCW * 5) - 8, (MCW * 5) + 8, (MCW * 6) - 7, (MCW * 6) + 7, (MCW * 7) - 7, (MCW * 7) - 6, (MCW * 7) + 6, (MCW * 7) + 7, (MCW * 8) - 5, (MCW * 8) - 4,
             (MCW * 8) + 4, (MCW * 8) + 5, (MCW * 9) - 3, (MCW * 9) - 2, (MCW * 9) + 2, (MCW * 9) + 3, (MCW * 10) - 1, (MCW * 10) + 0, (MCW * 10) + 1,
};

int const MapClass::RadiusCount[11] = {1, 9, 21, 37, 61, 89, 121, 161, 205, 253, 309};


void MapClass::One_Time(void) {
    GScreenClass::One_Time();
    
    // Default legacy map size
    XSize = MAP_CELL_W;
    YSize = MAP_CELL_H;
    Size = XSize * YSize;
    
    Alloc_Cells();
    
    CellTriggers.Resize(MAP_CELL_TOTAL);
}

void MapClass::Init_Clear(void) {
    GScreenClass::Init_Clear();
    Init_Cells();
    TiberiumScan = 0;
    IsForwardScan = true;
    TiberiumGrowthCount = 0;
    TiberiumSpreadCount = 0;
}

void MapClass::Alloc_Cells(void) {
    // Vector logic from GScreenClass / VectorClass
    Resize(Size);
}

void MapClass::Free_Cells(void) {
    Clear();
}

void MapClass::Init_Cells(void) {
    TotalValue = 0;
    
    // Clear the vector content
    // Assuming GScreenClass[] operator access:
    if (Count() < MAP_CELL_TOTAL) {
        Resize(MAP_CELL_TOTAL);
    }
    
    // Initialize defaults as per original MAP.CPP
    // Using operator[] from VectorClass
    for (int index = 0; index < MAP_CELL_TOTAL; index++) {
        CellClass& cell = (*this)[index];
        cell.Overlay = OVERLAY_NONE;
        cell.Smudge = SMUDGE_NONE;
        cell.TType = TEMPLATE_NONE;
        cell.Owner = HOUSE_NONE;
        cell.InfType = HOUSE_NONE;
        // memset(&cell, 0, sizeof(CellClass)); // Original did memset logic, but C++ objects might have vtables?
                                              // CellClass has simple layout but has methods. Memset is risky if vtable. 
                                              // Original code did memset.
        // For portability, element-wise reset is safer or relying on constructor.
        // Since Alloc_Cells calls Resize, new cells are default constructed.
        // Existing cells might need clearing.
        // Re-construct using placement new or just assign default object?
        // cell = CellClass(); // If valid copy assignment
    }
}

void MapClass::Set_Map_Dimensions(int x, int y, int w, int h) {
    MapCellX = x;
    MapCellY = y;
    MapCellWidth = w;
    MapCellHeight = h;
}

int MapClass::ID(void const* pointer) const {
    if (!pointer) return -1;
    if (Count() <= 0) return -1;

    // Address of first element
    const CellClass* base = &(*this)[0];
    const CellClass* ptr = static_cast<const CellClass*>(pointer);
    ptrdiff_t idx = ptr - base;
    if (idx < 0 || idx >= Count()) return -1;
    return static_cast<int>(idx);
}

// Stubs for logic not yet fully portable or dependent on complex systems
ObjectClass* MapClass::Close_Object(COORDINATE coord) const { return NULL; }
int MapClass::Cell_Region(CELL cell) { return 0; }
int MapClass::Cell_Threat(CELL cell, HousesType house) { return 0; }

int MapClass::Cell_Distance(CELL cell1, CELL cell2) {
    int x1 = Cell_X(cell1);
    int y1 = Cell_Y(cell1);
    int x2 = Cell_X(cell2);
    int y2 = Cell_Y(cell2);
    
    int x = x1 - x2;
    int y = y1 - y2;

    if (x < 0) x = -x;
    if (y < 0) y = -y;

    if (x > y) {
        return(x + (y >> 1));
    }
    return(y + (x >> 1));
}

bool MapClass::In_Radar(CELL cell) const {
    if (cell & 0xF000) return false; // Bounds check from original
    unsigned cx = static_cast<unsigned>(Cell_X(cell) - MapCellX);
    unsigned cy = static_cast<unsigned>(Cell_Y(cell) - MapCellY);
    return cx < static_cast<unsigned>(MapCellWidth) && cy < static_cast<unsigned>(MapCellHeight);
}

void MapClass::Sight_From(CELL cell, int sightrange, bool incremental) {
    if (!In_Radar(cell)) return;
    if (!sightrange || sightrange > 10) return;

    int xx = Cell_X(cell);
    int count = RadiusCount[sightrange];
    int const* ptr = &RadiusOffset[0];
    
    if (incremental) {
        if (sightrange > 1) {
            ptr += RadiusCount[sightrange - 2];
            count -= RadiusCount[sightrange - 2];
        }
    }

    while (count--) {
        CELL newcell = cell + *ptr++;
        
        if ((unsigned)newcell >= MAP_CELL_TOTAL) continue;
        int xdiff = Cell_X(newcell) - xx;
        if (abs(xdiff) > sightrange) continue;
        if (Distance(Cell_Coord(newcell), Cell_Coord(cell)) > (sightrange * 0x100)) continue; // Approximation of Distance logic or use Cell_Distance

        // Stub out map mapping for now as Map_Cell is missing from MapClass
        /*
        if (!(*this)[newcell].IsMapped) {
            Map.Map_Cell(newcell, PlayerPtr);
        }
        */
        // Simple shim logic:
        if (!(*this)[newcell].IsMapped) {
             (*this)[newcell].IsMapped = 1;
             // Call In_Radar or other logic if needed
        }
    }
}

void MapClass::Place_Down(CELL cell, ObjectClass* object) {}
void MapClass::Pick_Up(CELL cell, ObjectClass* object) {}
void MapClass::Overlap_Down(CELL cell, ObjectClass* object) {}
void MapClass::Overlap_Up(CELL cell, ObjectClass* object) {}
bool MapClass::Read_Binary(char const* root, unsigned long* crc) { return true; }
bool MapClass::Write_Binary(char const* root) { return false; }
bool MapClass::Place_Random_Crate(void) { return false; }
long MapClass::Overpass(void) { return 0; }
void MapClass::Logic(void) {}
void MapClass::Code_Pointers(void) {}
void MapClass::Decode_Pointers(void) {}
int MapClass::Validate(void) { return 1; }
