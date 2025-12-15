#include "function.h"
#include "toggle.h"

ToggleClass::ToggleClass(unsigned id, int x, int y, int w, int h)
    : ControlClass(id, x, y, w, h, LEFTPRESS | LEFTRELEASE, true) {
  IsPressed = false;
  IsOn = false;
  IsToggleType = false;
}

void ToggleClass::Turn_On() {
  IsOn = true;
  Flag_To_Redraw();
}

void ToggleClass::Turn_Off() {
  IsOn = false;
  Flag_To_Redraw();
}

int ToggleClass::Action(unsigned flags, KeyNumType& key) {
  if (flags == 0) {
    const bool hover = static_cast<unsigned>(Get_Mouse_X() - X) < static_cast<unsigned>(Width) &&
              static_cast<unsigned>(Get_Mouse_Y() - Y) < static_cast<unsigned>(Height);
    if (hover) {
      if (!IsPressed) {
        IsPressed = true;
        Flag_To_Redraw();
      }
    } else if (IsPressed) {
      IsPressed = false;
      Flag_To_Redraw();
    }
  }

  Sticky_Process(flags);

  if ((flags & LEFTPRESS) != 0) {
    IsPressed = true;
    Flag_To_Redraw();
    flags &= ~LEFTPRESS;
    ControlClass::Action(flags, key);
    key = KN_NONE;
    return true;
  }

  if ((flags & LEFTRELEASE) != 0) {
    if (IsPressed) {
      if (IsToggleType) {
        IsOn = !IsOn;
      }
      IsPressed = false;
    } else {
      flags &= ~LEFTRELEASE;
    }
  }

  return ControlClass::Action(flags, key);
}

