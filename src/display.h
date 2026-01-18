#pragma once

#include "gadget.h"
#include "layer.h"
#include "legacy_compat.h"
#include "map.h"

#ifndef CELL_LEPTON_W
#define CELL_LEPTON_W ICON_LEPTON_W
#endif
#ifndef CELL_LEPTON_H
#define CELL_LEPTON_H ICON_LEPTON_H
#endif

#ifndef PIXEL_LEPTON_W
#define PIXEL_LEPTON_W (ICON_LEPTON_W / ICON_PIXEL_W)
#endif
#ifndef PIXEL_LEPTON_H
#define PIXEL_LEPTON_H (ICON_LEPTON_H / ICON_PIXEL_H)
#endif

class DisplayClass : public MapClass {
public:
    TheaterType Theater;
    COORDINATE TacticalCoord;
    int TacLeptonWidth;
    int TacLeptonHeight;

    static LayerClass Layer[LAYER_COUNT];

    CELL ZoneCell;
    short ZoneOffset;
    short const* CursorSize;
    bool ProximityCheck;

    ObjectClass* PendingObjectPtr;
    ObjectTypeClass const* PendingObject;
    HousesType PendingHouse;

    static unsigned char FadingBrighten[256];
    static unsigned char FadingShade[256];
    static unsigned char FadingLight[256];
    static unsigned char RemapTables[HOUSE_COUNT][3][256];
    static unsigned char FadingGreen[256];
    static unsigned char FadingYellow[256];
    static unsigned char FadingRed[256];
    static unsigned char TranslucentTable[(MAGIC_COL_COUNT + 1) * 256];
    static unsigned char WhiteTranslucentTable[(1 + 1) * 256];
    static unsigned char MouseTranslucentTable[(4 + 1) * 256];
    static void const* TransIconset;
    static unsigned char UnitShadow[(USHADOW_COL_COUNT + 1) * 256];
    static unsigned char SpecialGhost[2 * 256];

    DisplayClass();

    virtual void Read_INI(char* buffer) { (void)buffer; }
    void Write_INI(char* buffer) { (void)buffer; }

    virtual void One_Time(void);
    virtual void Init_Clear(void);
    virtual void Init_IO(void);
    virtual void Init_Theater(TheaterType theater) { (void)theater; }

    virtual void AI(KeyNumType& input, int x, int y) {
        (void)input;
        (void)x;
        (void)y;
    }

    virtual void Draw_It(bool complete = false) { (void)complete; }

    void Center_Map(void) {}

    virtual bool Map_Cell(CELL cell, HouseClass* house) {
        (void)cell;
        (void)house;
        return false;
    }

    virtual CELL Click_Cell_Calc(int x, int y) {
        (void)x;
        (void)y;
        return -1;
    }

    virtual void Help_Text(int, int = -1, int = -1, int = YELLOW, bool = false, int = 0) {}
    virtual MouseType Get_Mouse_Shape(void) const = 0;

    virtual bool Scroll_Map(DirType facing, int& distance, bool really) {
        (void)facing;
        (void)distance;
        (void)really;
        return false;
    }

    virtual void Refresh_Cells(CELL cell, short const* list) {
        (void)cell;
        (void)list;
    }

    virtual void Set_View_Dimensions(int x, int y, int width = -1, int height = -1);

    virtual void Put_Place_Back(TechnoClass*) {}

    void Cursor_Mark(CELL pos, bool on);
    void Set_Cursor_Shape(short const* list);
    CELL Set_Cursor_Pos(CELL pos = -1);
    void Get_Occupy_Dimensions(int& w, int& h, short const* list);

    virtual void Set_Tactical_Position(COORDINATE coord) { TacticalCoord = coord; }
    void Refresh_Band(void) {}
    void Select_These(COORDINATE coord1, COORDINATE coord2) {
        (void)coord1;
        (void)coord2;
    }

    COORDINATE Pixel_To_Coord(int x, int y) {
        (void)x;
        (void)y;
        return 0;
    }

    bool Coord_To_Pixel(COORDINATE coord, int& x, int& y) {
        (void)coord;
        x = 0;
        y = 0;
        return false;
    }

    bool Push_Onto_TacMap(COORDINATE& source, COORDINATE& dest) {
        (void)source;
        (void)dest;
        return false;
    }

    void Remove(ObjectClass const* object, LayerType layer) {
        (void)object;
        (void)layer;
    }

    void Submit(ObjectClass const* object, LayerType layer) {
        (void)object;
        (void)layer;
    }

    CELL Calculated_Cell(SourceType dir, HousesType house) {
        (void)dir;
        (void)house;
        return -1;
    }

    bool In_View(CELL cell) {
        (void)cell;
        return false;
    }

    bool Passes_Proximity_Check(ObjectTypeClass const* object) {
        (void)object;
        return true;
    }

    ObjectClass* Cell_Object(CELL cell, int x = 0, int y = 0) {
        (void)cell;
        (void)x;
        (void)y;
        return NULL;
    }

    ObjectClass* Next_Object(ObjectClass* object) {
        (void)object;
        return NULL;
    }

    ObjectClass* Prev_Object(ObjectClass* object) {
        (void)object;
        return NULL;
    }

    int Cell_Shadow(CELL cell) {
        (void)cell;
        return 0;
    }

    short const* Text_Overlap_List(char const* text, int x, int y, int lines = 1) {
        (void)text;
        (void)x;
        (void)y;
        (void)lines;
        return NULL;
    }

    bool Is_Spot_Free(COORDINATE coord) const {
        (void)coord;
        return true;
    }

    COORDINATE Closest_Free_Spot(COORDINATE coord, bool any = false) const {
        (void)any;
        return coord;
    }

    void Sell_Mode_Control(int control) { IsSellMode = control ? 1u : 0u; }
    void Repair_Mode_Control(int control) { IsRepairMode = control ? 1u : 0u; }

    void Flag_Cell(CELL cell) {
        Flag_To_Redraw(false);
        IsToRedraw = true;
        CellRedraw[cell] = 1u;
    }

    bool Is_Cell_Flagged(CELL cell) const { return CellRedraw.Is_True(cell); }

    void Compute_Start_Pos(void) {}

    virtual void Code_Pointers(void) {}
    virtual void Decode_Pointers(void) {}

protected:
    virtual void Mouse_Right_Press(void) {}
    virtual void Mouse_Left_Press(int x, int y) {
        (void)x;
        (void)y;
    }
    virtual void Mouse_Left_Up(bool shadow, ObjectClass* object, ActionType action, bool wwsmall = false) {
        (void)shadow;
        (void)object;
        (void)action;
        (void)wwsmall;
    }
    virtual void Mouse_Left_Held(int x, int y) {
        (void)x;
        (void)y;
    }
    virtual void Mouse_Left_Release(CELL cell,
                                    int x,
                                    int y,
                                    ObjectClass* object,
                                    ActionType action,
                                    bool wwsmall = false) {
        (void)cell;
        (void)x;
        (void)y;
        (void)object;
        (void)action;
        (void)wwsmall;
    }

public:
    int TacPixelX;
    int TacPixelY;

    COORDINATE DesiredTacticalCoord;

    unsigned IsToRedraw : 1;
    unsigned IsRepairMode : 1;
    unsigned IsSellMode : 1;
    unsigned IsTargettingMode : 2;

protected:
    unsigned IsRubberBand : 1;
    unsigned IsTentative : 1;

    class TacticalClass : public GadgetClass {
    public:
        TacticalClass()
            : GadgetClass(0,
                          0,
                          0,
                          0,
                          LEFTPRESS | LEFTRELEASE | LEFTHELD | LEFTUP | RIGHTPRESS,
                          true) {}

    protected:
        virtual int Action(unsigned flags, KeyNumType& key);
    };

    friend class TacticalClass;

    static TacticalClass TacButton;

private:
    unsigned IsShadowPresent : 1;
    int BandX;
    int BandY;
    int NewX;
    int NewY;

    static void const* ShadowShapes;
    static unsigned char ShadowTrans[(SHADOW_COL_COUNT + 1) * 256];

    void Redraw_Icons(int draw_flags = 0) { (void)draw_flags; }
    void Redraw_Shadow(void) {}
    void Redraw_Shadow_Rects(void) {}

    static BooleanVectorClass CellRedraw;
};
