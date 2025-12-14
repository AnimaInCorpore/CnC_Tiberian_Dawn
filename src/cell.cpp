/*
**	Command & Conquer(tm)
**	Ported cell implementation (partial)
*/

#include "legacy/function.h"

/*
**	Note: This is a focused port containing the most-used CellClass
**	helpers required by other modules. Additional helpers from the
**	original CELL.CPP will be added incrementally.
*/

int CellClass::Validate(void) const
{
#ifdef CHEAT_KEYS
    int num = Cell_Number();
    if (num < 0 || num > 4095) {
        Validate_Error("CELL");
        return 0;
    }
    return 1;
#else
    return 1;
#endif
}

CellClass::CellClass(void)
{
    memset(this, 0, sizeof(CellClass));
    Smudge = SMUDGE_NONE;
    Overlay = OVERLAY_NONE;
    Smudge = SMUDGE_NONE;
    TType = TEMPLATE_NONE;
    Owner = HOUSE_NONE;
    InfType = HOUSE_NONE;
}

int CellClass::Cell_Color(bool override) const
{
    Validate();
    BuildingClass *object = Cell_Building();
    if (object) {
        return object->House->Class->Color;
    }

    if (override) {
        return TBLACK;
    }
    return (::Ground[Land_Type()].Color);
}

TechnoClass * CellClass::Cell_Techno(int x, int y) const
{
    Validate();
    ObjectClass *object;
    COORDINATE click = XY_Coord(Pixel_To_Lepton(x), Pixel_To_Lepton(y));
    TechnoClass *close = nullptr;
    long distance = 0;

    if (Cell_Occupier()) {
        object = Cell_Occupier();
        while (object) {
            if (object->Is_Techno()) {
                COORDINATE coord = object->Center_Coord() & 0x00FF00FFL;
                long dist = Distance(coord, click);
                if (!close || dist < distance) {
                    close = (TechnoClass *)object;
                    distance = dist;
                }
            }
            object = object->Next;
        }
    }
    return close;
}

ObjectClass * CellClass::Cell_Find_Object(RTTIType rtti) const
{
    Validate();
    ObjectClass *object = Cell_Occupier();

    while (object) {
        if (object->What_Am_I() == rtti) {
            return object;
        }
        object = object->Next;
    }
    return nullptr;
}

BuildingClass * CellClass::Cell_Building(void) const
{
    Validate();
    return (BuildingClass *)Cell_Find_Object(RTTI_BUILDING);
}

TerrainClass * CellClass::Cell_Terrain(void) const
{
    Validate();
    return (TerrainClass *)Cell_Find_Object(RTTI_TERRAIN);
}

ObjectClass * CellClass::Cell_Object(int x, int y) const
{
    Validate();

    ObjectClass *ptr = (ObjectClass *)Cell_Find_Object(RTTI_AIRCRAFT);
    if (ptr) return ptr;

    ptr = Cell_Techno(x, y);
    if (ptr) return ptr;
    ptr = Cell_Terrain();
    if (ptr) return ptr;
    return ptr;
}

void CellClass::Redraw_Objects(bool forced)
{
    Validate();
    CELL cell = Cell_Number();

    if (Map.In_View(cell) && (forced || !Map[cell].IsFlagged)) {
        Map.Flag_Cell(cell);

        if (Cell_Occupier()) {
            ObjectClass *optr = Cell_Occupier();
            while (optr) {
                optr->Mark(MARK_CHANGE);
                optr = optr->Next;
            }
        }

        for (int index = 0; index < (int)(sizeof(Overlapper)/sizeof(Overlapper[0])); index++) {
            if (Overlapper[index]) {
                Overlapper[index]->Mark(MARK_CHANGE);
            }
        }
    }
}
