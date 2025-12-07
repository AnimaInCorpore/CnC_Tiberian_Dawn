#include "legacy/map_shim.h"
#include "legacy/function.h"

MapStubClass::MapStubClass()
    : PendingHouse(HOUSE_NONE),
      PendingObject(nullptr),
      PendingObjectPtr(nullptr),
      MapCellX(0),
      MapCellY(0),
      MapCellWidth(0),
      MapCellHeight(0),
      TacPixelX(0),
      TacPixelY(0),
      TacLeptonWidth(0),
      TacLeptonHeight(0),
      TacticalCoord(0),
      DesiredTacticalCoord(0),
      SpecialRadarFrame(0),
      IsToRedraw(0),
      IsSellMode(false),
      IsTargettingMode(false),
      IsRadarActive(false),
      DoesRadarExist(false),
      IsSidebarActive(false),
      IsZoomed(false),
      PlayerNamesEnabled(false),
      FadingShade(nullptr),
      UnitShadow(nullptr),
      FadingLight(nullptr) {}

MapCellStub& MapStubClass::operator[](CELL cell) {
  return cells_[cell];
}

MapCellStub const& MapStubClass::operator[](CELL cell) const {
  return Lookup(cell);
}

void MapStubClass::Pick_Up(CELL, ObjectClass*) {}

void MapStubClass::Place_Down(CELL, ObjectClass*) {}

void MapStubClass::Refresh_Cells(CELL, short const*) {}

void MapStubClass::Flag_To_Redraw(bool complete) {
  IsToRedraw = complete ? 1u : IsToRedraw;
}

bool MapStubClass::Push_Onto_TacMap(COORDINATE, COORDINATE) { return false; }

bool MapStubClass::Coord_To_Pixel(COORDINATE, int& x, int& y) const {
  x = 0;
  y = 0;
  return false;
}

bool MapStubClass::In_Radar(CELL) const { return true; }

void MapStubClass::Sight_From(CELL, int, bool) {}

CELL MapStubClass::Calculated_Cell(SourceType, HousesType) { return 0; }

ObjectClass* MapStubClass::Cell_Object(CELL, int, int) { return nullptr; }

void MapStubClass::Detach(ObjectClass*) {}

void MapStubClass::Overlap_Down(CELL, ObjectClass*) {}

void MapStubClass::Overlap_Up(CELL, ObjectClass*) {}

void MapStubClass::Remove(ObjectClass*, int) {}

void MapStubClass::Submit(ObjectClass*, int) {}

void MapStubClass::Add(RTTIType, int) {}

void MapStubClass::Recalc() {}

bool MapStubClass::Abandon_Production(RTTIType, int) { return false; }

bool MapStubClass::Add_To_List(ObjectTypeClass const*) { return false; }

bool MapStubClass::Factory_Link(int, RTTIType, int) { return false; }

bool MapStubClass::Repair_Mode_Control(int) { return false; }

bool MapStubClass::Sell_Mode_Control(int) {
  IsSellMode = false;
  return false;
}

bool MapStubClass::Radar_Activate(bool activate) {
  IsRadarActive = activate;
  DoesRadarExist = activate;
  return IsRadarActive;
}

void MapStubClass::Render(bool) {}

void MapStubClass::Flag_Cell(CELL) {}

void MapStubClass::Set_Cursor_Pos(int, int) {}

void MapStubClass::Set_Cursor_Pos(CELL) {}

void MapStubClass::Set_Cursor_Shape(short const*) {}

void MapStubClass::Set_Default_Mouse(MouseType, bool) {}

void MapStubClass::Set_Tactical_Position(COORDINATE coord) {
  TacticalCoord = coord;
}

bool MapStubClass::Activate(int) { return false; }

bool MapStubClass::Click_In_Radar(int, int, bool) { return false; }

CELL MapStubClass::Click_Cell_Calc(int, int) { return 0; }

void MapStubClass::Mouse_Left_Release(CELL, int, int, ObjectClass*, ActionType, bool) {}

void MapStubClass::Mouse_Left_Up(bool, ObjectClass*, ActionType, bool) {}

void MapStubClass::Mouse_Right_Press() {}

bool MapStubClass::Override_Mouse_Shape(MouseType, bool) { return false; }

bool MapStubClass::Player_Names(bool redraw) {
  if (redraw) {
    IsToRedraw = 1;
    SpecialRadarFrame = SpecialRadarFrame ? SpecialRadarFrame : 1;
  }
  PlayerNamesEnabled = true;
  return PlayerNamesEnabled;
}

bool MapStubClass::Is_Player_Names() const { return PlayerNamesEnabled; }

bool MapStubClass::Is_Radar_Active() const { return IsRadarActive; }

bool MapStubClass::Is_Radar_Existing() const { return DoesRadarExist; }

int MapStubClass::Get_Tab_Height() const { return 0; }

void MapStubClass::Help_Text(int, int, int, int, bool, int) {}

void MapStubClass::Zoom_Mode(CELL cell) {
  IsZoomed = true;
  TacticalCoord = Cell_Coord(cell);
}

MapCellStub const& MapStubClass::Lookup(CELL cell) const {
  auto it = cells_.find(cell);
  if (it != cells_.end()) {
    return it->second;
  }
  static MapCellStub dummy;
  return dummy;
}
