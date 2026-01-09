#include "display.h"

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

int DisplayClass::TacticalClass::Action(unsigned flags, KeyNumType& key)
{
    (void)flags;
    (void)key;
    return 0;
}

