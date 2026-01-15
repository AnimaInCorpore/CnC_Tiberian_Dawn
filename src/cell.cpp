#include "cell.h"
#include "map.h"

#include <cstring>
#include <cstdlib>

static COORDINATE Make_Spot_From_Base(unsigned base, int spot)
{
    unsigned v = base;
    switch (spot) {
    case 0: v |= 0x00800080u; break; // center
    case 1: v |= 0x00400040u; break; // NW
    case 2: v |= 0x004000C0u; break; // NE
    case 3: v |= 0x00C00040u; break; // SW
    case 4: v |= 0x00C000C0u; break; // SE
    default: v |= 0x00800080u; break;
    }
    return static_cast<COORDINATE>(v);
}

CellClass::CellClass(void)
    : IsPlot(0),
      IsCursorHere(0),
      IsMapped(0),
      IsVisible(0),
      IsTrigger(0),
      IsWaypoint(0),
      IsRadarCursor(0),
      IsFlagged(0),
      TType(TEMPLATE_CLEAR1),
      TIcon(0),
      Overlay(OVERLAY_NONE),
      OverlayData(0),
      Smudge(SMUDGE_NONE),
      SmudgeData(0),
      Owner(HOUSE_NONE),
      InfType(HOUSE_NONE),
      OccupierPtr(NULL),
      Land(LAND_CLEAR) {
    Overlapper[0] = NULL;
    Overlapper[1] = NULL;
    Overlapper[2] = NULL;
    Flag.Composite = 0;
}

ObjectClass* CellClass::Cell_Occupier(void) const { return OccupierPtr; }

int CellClass::Spot_Index(COORDINATE coord) {
    int lx = Coord_XLepton(coord);
    int ly = Coord_YLepton(coord);

    // center tolerance area -> center
    const int center = 128;
    const int tol = 64;
    if (std::abs(lx - center) <= tol && std::abs(ly - center) <= tol) return 0; // Center

    // Quadrants: NW, NE, SW, SE
    if (lx < center && ly < center) return 1; // NW
    if (lx >= center && ly < center) return 2; // NE
    if (lx < center && ly >= center) return 3; // SW
    return 4; // SE
}

COORDINATE CellClass::Closest_Free_Spot(COORDINATE coord, bool any) const {
    // Normalize base cell (clear lepton bytes)
    unsigned base = static_cast<unsigned>(coord) & 0xFF00FF00u;

    // Helper to construct a coordinate for a spot index
    int start = Spot_Index(coord);
    if (start < 0) start = 0;

    // Check requested spot first
    if (Is_Spot_Free(start)) return Make_Spot_From_Base(base, start);

    // If any free spot is acceptable, try center then corners
    const int order[] = {0, 1, 2, 3, 4};
    for (int i = 0; i < 5; ++i) {
        if (Is_Spot_Free(order[i])) return Make_Spot_From_Base(base, order[i]);
    }

    // Fallback to original coordinate
    return coord;
}

bool CellClass::Is_Generally_Clear(void) const { 
    if (OccupierPtr) return false;
    if (Overlapper[0] || Overlapper[1] || Overlapper[2]) return false;
    return true; }

TARGET CellClass::As_Target(void) const { return static_cast<TARGET>(Cell_Number()); }

BuildingClass* CellClass::Cell_Building(void) const {
    if (OccupierPtr && OccupierPtr->What_Am_I() == RTTI_BUILDING) {
        return static_cast<BuildingClass*>(OccupierPtr);
    }
    return NULL;
}

CellClass const& CellClass::Adjacent_Cell(FacingType face) const {
    if (face == FACING_NONE) return *this;

    CELL num = Cell_Number();
    if ((unsigned)num >= (unsigned)MAP_CELL_TOTAL) return *this;

    CELL offset = AdjacentCell[face];
    int target = static_cast<int>(num) + static_cast<int>(offset);
    if (target < 0 || target >= MAP_CELL_TOTAL) return *this;

    return Map[target];
}

COORDINATE CellClass::Cell_Coord(void) const { return ::Cell_Coord(Cell_Number()); }

int CellClass::Cell_Color(bool override) const {
    (void)override;
    if (Land >= 0 && Land < LAND_COUNT) return Ground[Land].RadarColor;
    return 0;
}

CELL CellClass::Cell_Number(void) const { return static_cast<CELL>(Map.ID(this)); }

ObjectClass* CellClass::Cell_Find_Object(RTTIType rtti) const {
    ObjectClass* object = OccupierPtr;
    while (object) {
        if (object->What_Am_I() == rtti) return object;
        object = object->Next;
    }
    for (int i = 0; i < 3; ++i) {
        object = Overlapper[i];
        while (object) {
            if (object->What_Am_I() == rtti) return object;
            object = object->Next;
        }
    }
    return NULL;
}

ObjectClass* CellClass::Cell_Object(int x, int y) const {
    (void)x;
    (void)y;
    return OccupierPtr;
}

TechnoClass* CellClass::Cell_Techno(int x, int y) const {
    (void)x;
    (void)y;
    ObjectClass* object = OccupierPtr;
    while (object) {
        RTTIType r = object->What_Am_I();
        if (r == RTTI_UNIT || r == RTTI_INFANTRY || r == RTTI_AIRCRAFT) return static_cast<TechnoClass*>(object);
        object = object->Next;
    }
    for (int i = 0; i < 3; ++i) {
        object = Overlapper[i];
        while (object) {
            RTTIType r = object->What_Am_I();
            if (r == RTTI_UNIT || r == RTTI_INFANTRY || r == RTTI_AIRCRAFT) return static_cast<TechnoClass*>(object);
            object = object->Next;
        }
    }
    return NULL;
}

TerrainClass* CellClass::Cell_Terrain(void) const {
    // Terrain RTTI isn't available in the lightweight shim; return NULL for now.
    (void)0;
    return NULL;
}

UnitClass* CellClass::Cell_Unit(void) const {
    ObjectClass* obj = Cell_Find_Object(RTTI_UNIT);
    return obj ? reinterpret_cast<UnitClass*>(obj) : NULL;
}

InfantryClass* CellClass::Cell_Infantry(void) const {
    ObjectClass* obj = Cell_Find_Object(RTTI_INFANTRY);
    return obj ? reinterpret_cast<InfantryClass*>(obj) : NULL;
}

TriggerClass* CellClass::Get_Trigger(void) const {
    int num = Cell_Number();
    if (num < 0 || num >= MAP_CELL_TOTAL) return NULL;
    extern DynamicVectorClass<TriggerClass*> CellTriggers;
    if (CellTriggers.Count() <= num) return NULL;
    return CellTriggers[num];
}

int CellClass::Clear_Icon(void) const {
    Validate();
    CELL cell = Cell_Number();
    return ((cell & 0x03) | ((cell >> 4) & 0x0C));
}

bool CellClass::Goodie_Check(FootClass* object) {
    (void)object;
    return false;
}

ObjectClass* CellClass::Fetch_Occupier(void) const { return OccupierPtr; }

void CellClass::Occupy_Down(ObjectClass* object) {
    OccupierPtr = object;
    Flag.Occupy.Center = object ? 1U : 0U;
}

void CellClass::Occupy_Up(ObjectClass* object) {
    if (!object || object == OccupierPtr) {
        OccupierPtr = NULL;
        Flag.Occupy.Center = 0;
    }
}

void CellClass::Overlap_Down(ObjectClass* object) {
    for (int i = 0; i < 3; ++i) {
        if (!Overlapper[i]) {
            Overlapper[i] = object;
            return;
        }
    }
}

void CellClass::Overlap_Up(ObjectClass* object) {
    for (int i = 0; i < 3; ++i) {
        if (Overlapper[i] == object) {
            Overlapper[i] = NULL;
            return;
        }
    }
}

bool CellClass::Flag_Place(HousesType house) {
    Owner = house;
    IsFlagged = 1;
    return true;
}

bool CellClass::Flag_Remove(void) {
    Owner = HOUSE_NONE;
    IsFlagged = 0;
    return true;
}

bool CellClass::Should_Save(void) const { return true; }

bool CellClass::Save(FileClass& file) {
    (void)file;
    return true;
}

bool CellClass::Load(FileClass& file) {
    (void)file;
    return true;
}

void CellClass::Code_Pointers(void) {}

void CellClass::Decode_Pointers(void) {}

void CellClass::Draw_It(int x, int y, int draw_flags) const {
    (void)x;
    (void)y;
    (void)draw_flags;
}

void CellClass::Redraw_Objects(bool forced) { (void)forced; }

void CellClass::Shimmer(void) {}

long CellClass::Tiberium_Adjust(bool pregame) {
    (void)pregame;
    return 0;
}

void CellClass::Wall_Update(void) {}

void CellClass::Concrete_Calc(void) {}

void CellClass::Recalc_Attributes(void) {}

int CellClass::Reduce_Tiberium(int levels) {
    (void)levels;
    return 0;
}

int CellClass::Reduce_Wall(int damage) {
    (void)damage;
    return 0;
}

void CellClass::Incoming(COORDINATE threat, bool forced) {
    (void)threat;
    (void)forced;
}

void CellClass::Adjust_Threat(HousesType house, int threat_value) {
    (void)house;
    (void)threat_value;
}

int CellClass::Validate(void) const { return 1; }

