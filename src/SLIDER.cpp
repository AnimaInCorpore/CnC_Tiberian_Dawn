#include <algorithm>

#include "function.h"
#include "slider.h"

SliderClass::SliderClass(unsigned id, int x, int y, int w, int h, int belong_to_list)
    : GaugeClass(id, x, y, w, h) {
  BelongToList = belong_to_list ? true : false;
  PlusGadget = nullptr;
  MinusGadget = nullptr;

  if (!BelongToList) {
    PlusGadget = new ShapeButtonClass(id, MixFileClass::Retrieve("BTN-PLUS.SHP"), X + Width + 2, Y);
    MinusGadget = new ShapeButtonClass(id, MixFileClass::Retrieve("BTN-MINS.SHP"), X - 6, Y);

    if (PlusGadget) {
      PlusGadget->Make_Peer(*this);
      PlusGadget->Add(*this);
      PlusGadget->Flag_To_Redraw();
    }
    if (MinusGadget) {
      MinusGadget->Make_Peer(*this);
      MinusGadget->Add(*this);
      MinusGadget->Flag_To_Redraw();
    }
  }

  Thumb = 1;
  ThumbSize = 0;
  ThumbStart = 0;
  Set_Thumb_Size(1);
  Recalc_Thumb();
  IsColorized = false;
}

SliderClass::~SliderClass() {
  delete PlusGadget;
  PlusGadget = nullptr;
  delete MinusGadget;
  MinusGadget = nullptr;
}

int SliderClass::Set_Maximum(int value) {
  if (!GaugeClass::Set_Maximum(value)) {
    return false;
  }
  Recalc_Thumb();
  return true;
}

void SliderClass::Set_Thumb_Size(int value) {
  Thumb = std::max(1, std::min(value, MaxValue));
  Flag_To_Redraw();
  Recalc_Thumb();
}

int SliderClass::Set_Value(int value) {
  const int limit = std::max(MaxValue - Thumb, 0);
  value = std::min(value, limit);
  if (!GaugeClass::Set_Value(value)) {
    return false;
  }
  Recalc_Thumb();
  return true;
}

void SliderClass::Recalc_Thumb() {
  const int length = IsHorizontal ? Width : Height;
  if (length <= 0 || MaxValue <= 0) {
    ThumbSize = length;
    ThumbStart = 0;
    return;
  }

  const int size = Fixed_To_Cardinal(length, Cardinal_To_Fixed(MaxValue, Thumb));
  ThumbSize = std::max(size, 4);

  const int start =
      Fixed_To_Cardinal(length, Cardinal_To_Fixed(MaxValue, Bound(CurValue, 0, MaxValue)));
  ThumbStart = std::min(start, length - ThumbSize);
}

int SliderClass::Action(unsigned flags, KeyNumType& key) {
  if (flags & LEFTPRESS) {
    const int mouse = IsHorizontal ? Get_Mouse_X() : Get_Mouse_Y();
    const int edge = (IsHorizontal ? X : Y) + 1;
    const int thumb_end = edge + ThumbStart + ThumbSize;

    if (mouse < edge + ThumbStart) {
      Bump(true);
      GaugeClass::Action(0, key);
      key = KN_NONE;
      return true;
    }
    if (mouse > thumb_end) {
      Bump(false);
      GaugeClass::Action(0, key);
      key = KN_NONE;
      return true;
    }

    GaugeClass::Action(flags, key);
    key = KN_NONE;
    return true;
  }

  return GaugeClass::Action(flags, key);
}

int SliderClass::Bump(int up) {
  return Set_Value(up ? (CurValue - Thumb) : (CurValue + Thumb));
}

int SliderClass::Step(int up) { return Set_Value(up ? (CurValue - 1) : (CurValue + 1)); }

void SliderClass::Draw_Thumb() {
  if (IsHorizontal) {
    Draw_Box(X + ThumbStart, Y, ThumbSize, Height, BOXSTYLE_GREEN_RAISED, true);
  } else {
    Draw_Box(X, Y + ThumbStart, Width, ThumbSize, BOXSTYLE_GREEN_RAISED, true);
  }
}

int SliderClass::Draw_Me(int forced) {
  if (BelongToList) {
    if (!ControlClass::Draw_Me(forced)) {
      return false;
    }

    if (LogicPage == &SeenBuff) {
      Conditional_Hide_Mouse(X, Y, X + Width, Y + Height);
    }

    Draw_Box(X, Y, Width, Height, BOXSTYLE_GREEN_DOWN, true);
    Draw_Thumb();

    if (LogicPage == &SeenBuff) {
      Conditional_Show_Mouse();
    }

    return true;
  }

  return GaugeClass::Draw_Me(forced);
}

void SliderClass::Peer_To_Peer(unsigned flags, KeyNumType&, ControlClass& whom) {
  if (!(flags & LEFTRELEASE)) {
    return;
  }

  if (&whom == PlusGadget) {
    Step(false);
  } else if (&whom == MinusGadget) {
    Step(true);
  }
}
