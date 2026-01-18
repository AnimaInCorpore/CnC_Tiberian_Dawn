#include "display.h"

#include <cstring>

LayerClass DisplayClass::Layer[LAYER_COUNT];

unsigned char DisplayClass::FadingBrighten[256];
unsigned char DisplayClass::FadingShade[256];
unsigned char DisplayClass::FadingLight[256];
unsigned char DisplayClass::RemapTables[HOUSE_COUNT][3][256];
unsigned char DisplayClass::FadingGreen[256];
unsigned char DisplayClass::FadingYellow[256];
unsigned char DisplayClass::FadingRed[256];
unsigned char DisplayClass::TranslucentTable[(MAGIC_COL_COUNT + 1) * 256];
unsigned char DisplayClass::WhiteTranslucentTable[(1 + 1) * 256];
unsigned char DisplayClass::MouseTranslucentTable[(4 + 1) * 256];
void const* DisplayClass::TransIconset = NULL;
unsigned char DisplayClass::UnitShadow[(USHADOW_COL_COUNT + 1) * 256];
unsigned char DisplayClass::SpecialGhost[2 * 256];

void const* DisplayClass::ShadowShapes = NULL;
unsigned char DisplayClass::ShadowTrans[(SHADOW_COL_COUNT + 1) * 256];

BooleanVectorClass DisplayClass::CellRedraw;
DisplayClass::TacticalClass DisplayClass::TacButton;

DisplayClass::DisplayClass()
    : Theater(THEATER_NONE),
      TacticalCoord(0),
      TacLeptonWidth(0),
      TacLeptonHeight(0),
      ZoneCell(0),
      ZoneOffset(0),
      CursorSize(NULL),
      ProximityCheck(false),
      PendingObjectPtr(NULL),
      PendingObject(NULL),
      PendingHouse(HOUSE_NONE),
      TacPixelX(0),
      TacPixelY(0),
      DesiredTacticalCoord(0),
      IsToRedraw(1u),
      IsRepairMode(0u),
      IsSellMode(0u),
      IsTargettingMode(0u),
      IsRubberBand(0u),
      IsTentative(0u),
      IsShadowPresent(0u),
      BandX(0),
      BandY(0),
      NewX(0),
      NewY(0) {}

void DisplayClass::One_Time(void)
{
    /*
    ** Keep this lightweight for now: the full tactical redraw pipeline is
    ** still being ported, but we must ensure global tables are initialized
    ** deterministically (avoid uninitialized palette table reads).
    */
    Set_View_Dimensions(0, 0);

    MapClass::One_Time();

    CellRedraw.Resize(MAP_CELL_TOTAL);

    for (int layer = 0; layer < LAYER_COUNT; ++layer) {
        Layer[layer].One_Time();
    }

    TransIconset = MixFileClass::Retrieve("TRANS.ICN");
    ShadowShapes = MixFileClass::Retrieve("SHADOW.SHP");

    for (int i = 0; i < 256; ++i) {
        FadingBrighten[i] = static_cast<unsigned char>(i);
        FadingShade[i] = static_cast<unsigned char>(i);
        FadingLight[i] = static_cast<unsigned char>(i);
        FadingGreen[i] = static_cast<unsigned char>(i);
        FadingYellow[i] = static_cast<unsigned char>(i);
        FadingRed[i] = static_cast<unsigned char>(i);
    }

    for (int fade = 0; fade < 3; ++fade) {
        for (int house = 0; house < HOUSE_COUNT; ++house) {
            switch (fade) {
            case 0:
                for (int color = 0; color < 256; ++color) {
                    RemapTables[house][fade][color] = static_cast<unsigned char>(color);
                }
                break;
            case 1: Mem_Copy(FadingLight, RemapTables[house][fade], 256); break;
            case 2: Mem_Copy(FadingShade, RemapTables[house][fade], 256); break;
            default: break;
            }

            Mem_Copy(&RemapTables[house][fade][(house + 11) * 16],
                     &RemapTables[house][fade][(0 + 11) * 16],
                     16);
        }
    }

    for (int group = 0; group < (MAGIC_COL_COUNT + 1); ++group) {
        for (int color = 0; color < 256; ++color) {
            TranslucentTable[(group * 256) + color] = static_cast<unsigned char>(color);
        }
    }

    for (int group = 0; group < (1 + 1); ++group) {
        for (int color = 0; color < 256; ++color) {
            WhiteTranslucentTable[(group * 256) + color] = static_cast<unsigned char>(color);
        }
    }

    for (int group = 0; group < (4 + 1); ++group) {
        for (int color = 0; color < 256; ++color) {
            MouseTranslucentTable[(group * 256) + color] = static_cast<unsigned char>(color);
        }
    }

    for (int group = 0; group < (SHADOW_COL_COUNT + 1); ++group) {
        for (int color = 0; color < 256; ++color) {
            ShadowTrans[(group * 256) + color] = static_cast<unsigned char>(color);
        }
    }

    for (int group = 0; group < (USHADOW_COL_COUNT + 1); ++group) {
        for (int color = 0; color < 256; ++color) {
            UnitShadow[(group * 256) + color] = static_cast<unsigned char>(color);
        }
    }

    for (int color = 0; color < 256; ++color) {
        SpecialGhost[color] = 0;
        SpecialGhost[color + 256] = static_cast<unsigned char>(color);
    }
}

void DisplayClass::Init_Clear(void)
{
    MapClass::Init_Clear();

    Theater = THEATER_NONE;
    TacticalCoord = 0;
    ZoneCell = 0;
    ZoneOffset = 0;
    CursorSize = NULL;
    ProximityCheck = false;
    PendingObjectPtr = NULL;
    PendingObject = NULL;
    PendingHouse = HOUSE_NONE;
    DesiredTacticalCoord = 0;

    IsToRedraw = 1u;
    IsRepairMode = 0u;
    IsSellMode = 0u;
    IsTargettingMode = 0u;
    IsRubberBand = 0u;
    IsTentative = 0u;
    IsShadowPresent = 0u;

    BandX = 0;
    BandY = 0;
    NewX = 0;
    NewY = 0;

    CellRedraw.Reset();

    Flag_To_Redraw(true);
}

void DisplayClass::Init_IO(void)
{
    GScreenClass::Init_IO();
    Add_A_Button(TacButton);
}

void DisplayClass::Set_View_Dimensions(int x, int y, int width, int height)
{
    if (width == -1) {
        width = SeenBuff.Get_Width() - x;
    }
    if (height == -1) {
        height = SeenBuff.Get_Height() - y;
    }

    TacLeptonWidth = Pixel_To_Lepton(width);
    TacLeptonHeight = Pixel_To_Lepton(height);

    TacPixelX = x;
    TacPixelY = y;

    IsToRedraw = 1u;
    Flag_To_Redraw(false);

    TacButton.X = TacPixelX;
    TacButton.Y = TacPixelY;
    TacButton.Width = Lepton_To_Pixel(TacLeptonWidth);
    TacButton.Height = Lepton_To_Pixel(TacLeptonHeight);
}

void DisplayClass::Cursor_Mark(CELL pos, bool on)
{
    if (pos == -1) return;
    if (!CursorSize) return;

    short const* ptr = CursorSize;
    while (*ptr != REFRESH_EOL) {
        CELL cell = static_cast<CELL>(pos + *ptr++);
        if (cell < 0 || cell >= MAP_CELL_TOTAL) continue;

        CellClass& cellref = (*this)[cell];
        cellref.IsCursorHere = on ? 1u : 0u;
        cellref.Redraw_Objects(true);
        Flag_Cell(cell);
    }
}

void DisplayClass::Set_Cursor_Shape(short const* list)
{
    if (CursorSize) {
        Cursor_Mark(static_cast<CELL>(ZoneCell + ZoneOffset), false);
    }

    ZoneOffset = 0;

    if (!list) {
        CursorSize = NULL;
        return;
    }

    static short cursor_list[50];
    int out = 0;
    while (out < static_cast<int>(sizeof(cursor_list) / sizeof(cursor_list[0])) - 1) {
        cursor_list[out] = list[out];
        if (cursor_list[out] == REFRESH_EOL) break;
        ++out;
    }
    cursor_list[sizeof(cursor_list) / sizeof(cursor_list[0]) - 1] = REFRESH_EOL;

    CursorSize = cursor_list;

    int w = 0;
    int h = 0;
    Get_Occupy_Dimensions(w, h, CursorSize);
    ZoneOffset = static_cast<short>(-(((h / 2) * MAP_CELL_W) + (w / 2)));

    Cursor_Mark(static_cast<CELL>(ZoneCell + ZoneOffset), true);
}

CELL DisplayClass::Set_Cursor_Pos(CELL pos)
{
    if (pos == -1) return ZoneCell;

    if (CursorSize) {
        Cursor_Mark(static_cast<CELL>(ZoneCell + ZoneOffset), false);
    }

    ZoneCell = pos;

    if (CursorSize) {
        Cursor_Mark(static_cast<CELL>(ZoneCell + ZoneOffset), true);
    }

    return ZoneCell;
}

void DisplayClass::Get_Occupy_Dimensions(int& w, int& h, short const* list)
{
    w = 0;
    h = 0;
    if (!list) return;

    int min_x = MAP_CELL_W;
    int max_x = -MAP_CELL_W;
    int min_y = MAP_CELL_H;
    int max_y = -MAP_CELL_H;

    while (*list != REFRESH_EOL) {
        int offset = *list++;
        int x = offset % MAP_CELL_W;
        int y = offset / MAP_CELL_W;

        if (x > max_x) max_x = x;
        if (x < min_x) min_x = x;
        if (y > max_y) max_y = y;
        if (y < min_y) min_y = y;
    }

    w = MAX(1, max_x - min_x + 1);
    h = MAX(1, max_y - min_y + 1);
}

int DisplayClass::TacticalClass::Action(unsigned flags, KeyNumType& key)
{
    (void)flags;
    (void)key;
    return 0;
}
