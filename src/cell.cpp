#include "cell.h"

#include <cstring>

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
    (void)coord;
    return 0;
}

COORDINATE CellClass::Closest_Free_Spot(COORDINATE coord, bool any) const {
    (void)any;
    return coord;
}

bool CellClass::Is_Generally_Clear(void) const { return true; }

TARGET CellClass::As_Target(void) const { return static_cast<TARGET>(Cell_Number()); }

BuildingClass* CellClass::Cell_Building(void) const {
    if (OccupierPtr && OccupierPtr->What_Am_I() == RTTI_BUILDING) {
        return static_cast<BuildingClass*>(OccupierPtr);
    }
    return NULL;
}

CellClass const& CellClass::Adjacent_Cell(FacingType face) const {
    (void)face;
    return *this;
}

COORDINATE CellClass::Cell_Coord(void) const { return ::Cell_Coord(Cell_Number()); }

int CellClass::Cell_Color(bool override) const {
    (void)override;
    return 0;
}

CELL CellClass::Cell_Number(void) const { return 0; }

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
    return NULL;
}

TerrainClass* CellClass::Cell_Terrain(void) const { return NULL; }

UnitClass* CellClass::Cell_Unit(void) const { return NULL; }

InfantryClass* CellClass::Cell_Infantry(void) const { return NULL; }

TriggerClass* CellClass::Get_Trigger(void) const { return NULL; }

int CellClass::Clear_Icon(void) const { return 0; }

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

