#include "legacy/mouse.h"
#include "legacy/function.h"

void const* MouseClass::MouseShapes = nullptr;
CountDownTimerClass MouseClass::Timer;

MouseClass::MouseStruct MouseClass::MouseControl[MOUSE_COUNT] = {
    {0, 1, 0, 86, 0, 0},          // MOUSE_NORMAL
    {1, 1, 0, -1, 15, 0},         // MOUSE_N
    {2, 1, 0, -1, 29, 0},         // MOUSE_NE
    {3, 1, 0, -1, 29, 12},        // MOUSE_E
    {4, 1, 0, -1, 29, 23},        // MOUSE_SE
    {5, 1, 0, -1, 15, 23},        // MOUSE_S
    {6, 1, 0, -1, 0, 23},         // MOUSE_SW
    {7, 1, 0, -1, 0, 13},         // MOUSE_W
    {8, 1, 0, -1, 0, 0},          // MOUSE_NW

    {130, 1, 0, -1, 15, 0},       // MOUSE_NO_N
    {131, 1, 0, -1, 29, 0},       // MOUSE_NO_NE
    {132, 1, 0, -1, 29, 12},      // MOUSE_NO_E
    {133, 1, 0, -1, 29, 23},      // MOUSE_NO_SE
    {134, 1, 0, -1, 15, 23},      // MOUSE_NO_S
    {135, 1, 0, -1, 0, 23},       // MOUSE_NO_SW
    {136, 1, 0, -1, 0, 13},       // MOUSE_NO_W
    {137, 1, 0, -1, 0, 0},        // MOUSE_NO_NW

    {11, 1, 0, 27, 15, 12},       // MOUSE_NO_MOVE
    {10, 1, 0, 26, 15, 12},       // MOUSE_CAN_MOVE
    {119, 3, 4, 148, 15, 12},     // MOUSE_ENTER
    {53, 9, 4, -1, 15, 12},       // MOUSE_DEPLOY
    {12, 6, 4, -1, 15, 12},       // MOUSE_CAN_SELECT
    {18, 8, 4, 140, 15, 12},      // MOUSE_CAN_ATTACK
    {62, 24, 2, -1, 15, 12},      // MOUSE_SELL_BACK
    {154, 24, 2, -1, 15, 12},     // MOUSE_SELL_UNIT
    {29, 24, 2, -1, 15, 12},      // MOUSE_REPAIR
    {126, 1, 0, -1, 15, 12},      // MOUSE_NO_REPAIR
    {125, 1, 0, -1, 15, 12},      // MOUSE_NO_SELL_BACK
    {87, 1, 0, 151, 0, 0},        // MOUSE_RADAR_CURSOR
    {103, 16, 2, -1, 15, 12},     // MOUSE_ION_CANNON
    {96, 7, 4, -1, 15, 12},       // MOUSE_NUCLEAR_BOMB
    {88, 8, 2, -1, 15, 12},       // MOUSE_AIR_STRIKE
    {122, 3, 4, 127, 15, 12},     // MOUSE_DEMOLITIONS
    {153, 1, 0, 152, 15, 12},     // MOUSE_AREA_GUARD
};

MouseClass::MouseClass(void) {
  CurrentMouseShape = MOUSE_NORMAL;
  NormalMouseShape = MOUSE_NORMAL;
  Timer.Start();
}

void MouseClass::One_Time(void) {
  ScrollClass::One_Time();

  RawFileClass file("MOUSE.SHP");
  if (file.Is_Available()) {
    MouseShapes = Load_Alloc_Data(file);
  } else {
    MouseShapes = MixFileClass::Retrieve("MOUSE.SHP");
  }
}

void MouseClass::Init_Clear(void) {
  ScrollClass::Init_Clear();
  IsSmall = false;
  NormalMouseShape = MOUSE_NORMAL;
}

void MouseClass::Set_Default_Mouse(MouseType mouse, bool wwsmall) {
  NormalMouseShape = mouse;
  Override_Mouse_Shape(mouse, wwsmall);
}

void MouseClass::Revert_Mouse_Shape(void) {
  Override_Mouse_Shape(NormalMouseShape, IsSmall);
}

void MouseClass::Mouse_Small(bool wwsmall) {
  Override_Mouse_Shape(CurrentMouseShape, wwsmall);
}

bool MouseClass::Override_Mouse_Shape(MouseType mouse, bool wwsmall) {
  MouseStruct const* control = &MouseControl[mouse];
  static bool startup = false;

  if (control->SmallFrame == -1) {
    wwsmall = false;
  }

  if (!startup || (MouseShapes && ((mouse != CurrentMouseShape) || (wwsmall != IsSmall)))) {
    startup = true;

    Timer.Set(control->FrameRate);
    Frame = 0;

    const int baseshp = (wwsmall) ? control->SmallFrame : control->StartFrame;
    Set_Mouse_Cursor(control->X, control->Y, Extract_Shape(MouseShapes, baseshp + (Frame / 4)));

    CurrentMouseShape = mouse;
    IsSmall = wwsmall;
    return true;
  }
  return false;
}

void MouseClass::AI(KeyNumType& input, int x, int y) {
  void* mouse_shape_ptr;
  MouseStruct const* control = &MouseControl[CurrentMouseShape];

  if (control->FrameRate && Timer.Time() == 0) {
    Frame++;
    Frame %= control->FrameCount;
    Timer.Set(control->FrameRate);

    if (!IsSmall || control->SmallFrame != -1) {
      const int baseframe = (IsSmall) ? control->SmallFrame : control->StartFrame;
      mouse_shape_ptr = Extract_Shape(MouseShapes, baseframe + Frame);
      if (mouse_shape_ptr) {
        Set_Mouse_Cursor(control->X, control->Y, mouse_shape_ptr);
      }
    }
  }

  ScrollClass::AI(input, x, y);
}
