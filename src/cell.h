#pragma once

#include "legacy_compat.h"

class InfantryClass;
class TechnoClass;
class TerrainClass;
class TriggerClass;
class UnitClass;

class CellClass {
public:
    unsigned IsPlot : 1;
    unsigned IsCursorHere : 1;
    unsigned IsMapped : 1;
    unsigned IsVisible : 1;
    unsigned IsTrigger : 1;
    unsigned IsWaypoint : 1;
    unsigned IsRadarCursor : 1;
    unsigned IsFlagged : 1;

    TemplateType TType;
    unsigned char TIcon;

    OverlayType Overlay;
    unsigned char OverlayData;

    SmudgeType Smudge;
    unsigned char SmudgeData;

    HousesType Owner;
    HousesType InfType;

    ObjectClass* OccupierPtr;
    ObjectClass* Overlapper[3];

    union {
        struct {
            unsigned Center : 1;
            unsigned NW : 1;
            unsigned NE : 1;
            unsigned SW : 1;
            unsigned SE : 1;
            unsigned Vehicle : 1;
            unsigned Monolith : 1;
            unsigned Building : 1;
        } Occupy;
        unsigned char Composite;
    } Flag;

    CellClass(void);
    ~CellClass(void) {}

    int operator==(CellClass const& cell) const { return &cell == this; }
    int operator!=(CellClass const&) const { return 0; }

    ObjectClass* Cell_Occupier(void) const;
    static int Spot_Index(COORDINATE coord);
    bool Is_Spot_Free(int spot_index) const { return (!(Flag.Composite & (1 << spot_index))); }
    COORDINATE Closest_Free_Spot(COORDINATE coord, bool any = false) const;
    COORDINATE Free_Spot(void) const { return Closest_Free_Spot(Cell_Coord()); }
    bool Is_Generally_Clear(void) const;
    TARGET As_Target(void) const;
    BuildingClass* Cell_Building(void) const;
    CellClass const& Adjacent_Cell(FacingType face) const;
    CellClass& Adjacent_Cell(FacingType face) {
        return const_cast<CellClass&>(static_cast<CellClass const*>(this)->Adjacent_Cell(face));
    }
    COORDINATE Cell_Coord(void) const;
    int Cell_Color(bool override = false) const;
    CELL Cell_Number(void) const;
    LandType Land_Type(void) const { return Land; }
    ObjectClass* Cell_Find_Object(RTTIType rtti) const;
    ObjectClass* Cell_Object(int x = 0, int y = 0) const;
    TechnoClass* Cell_Techno(int x = 0, int y = 0) const;
    TerrainClass* Cell_Terrain(void) const;
    UnitClass* Cell_Unit(void) const;
    InfantryClass* Cell_Infantry(void) const;
    TriggerClass* Get_Trigger(void) const;
    int Clear_Icon(void) const;
    bool Goodie_Check(FootClass* object);
    ObjectClass* Fetch_Occupier(void) const;

    void Occupy_Down(ObjectClass* object);
    void Occupy_Up(ObjectClass* object);
    void Overlap_Down(ObjectClass* object);
    void Overlap_Up(ObjectClass* object);
    bool Flag_Place(HousesType house);
    bool Flag_Remove(void);

    bool Should_Save(void) const;
    bool Save(FileClass& file);
    bool Load(FileClass& file);
    void Code_Pointers(void);
    void Decode_Pointers(void);

    void Draw_It(int x, int y, int draw_flags = 0) const;
    void Redraw_Objects(bool forced = false);
    void Shimmer(void);

    long Tiberium_Adjust(bool pregame = false);
    void Wall_Update(void);
    void Concrete_Calc(void);
    void Recalc_Attributes(void);
    int Reduce_Tiberium(int levels);
    int Reduce_Wall(int damage);
    void Incoming(COORDINATE threat = 0, bool forced = false);
    void Adjust_Threat(HousesType house, int threat_value);

    int Validate(void) const;

private:
    LandType Land;
};

