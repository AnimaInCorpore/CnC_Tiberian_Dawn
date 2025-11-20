#include "function.h"
#include "list.h"

#include <algorithm>

namespace {
int ClampIndex(int value, int min_value, int max_value) {
  if (max_value < min_value) {
    max_value = min_value;
  }
  if (value < min_value) {
    return min_value;
  }
  if (value > max_value) {
    return max_value;
  }
  return value;
}
}  // namespace

ListClass::ListClass(int id, int x, int y, int w, int h, TextPrintType flags,
                     void const* up, void const* down)
    : ControlClass(id, x, y, w, h, LEFTPRESS | LEFTRELEASE | KEYBOARD, false),
      UpGadget(0, up, x + w, y),
      DownGadget(0, down, x + w, y + h),
      ScrollGadget(0, x + w, y, 0, h, true) {
  UpGadget.X -= UpGadget.Width;
  DownGadget.X -= DownGadget.Width;
  DownGadget.Y -= DownGadget.Height;

  const int button_width = std::max(UpGadget.Width, DownGadget.Width);
  ScrollGadget.X -= button_width;
  ScrollGadget.Y = Y + UpGadget.Height;
  ScrollGadget.Height -= UpGadget.Height + DownGadget.Height;
  ScrollGadget.Width = button_width;

  TextFlags = flags;
  IsScrollActive = false;
  Tabs = nullptr;
  SelectedIndex = 0;
  CurrentTopIndex = 0;

  Fancy_Text_Print(TXT_NONE, 0, 0, TBLACK, TBLACK, TextFlags);
  LineHeight = FontHeight + FontYSpacing - 1;
  LineHeight = std::max(LineHeight, 1);
  LineCount = std::max(1, (h - 1) / LineHeight);
}

ListClass::~ListClass() {
  Remove_Scroll_Bar();
}

int ListClass::Add_Item(char const* text) {
  if (text) {
    List.Add(text);
    Flag_To_Redraw();

    if (List.Count() > LineCount) {
      Add_Scroll_Bar();
    }

    if (IsScrollActive) {
      ScrollGadget.Set_Maximum(List.Count());
    }
  }
  return List.Count() - 1;
}

int ListClass::Add_Item(int text) {
  if (text != TXT_NONE) {
    Add_Item(Text_String(text));
  }
  return List.Count() - 1;
}

void ListClass::Remove_Item(char const* text) {
  if (!text) {
    return;
  }
  List.Delete(text);

  if (List.Count() <= LineCount) {
    Remove_Scroll_Bar();
  }

  if (IsScrollActive) {
    ScrollGadget.Set_Maximum(List.Count());
  }

  SelectedIndex = std::min(SelectedIndex, List.Count() - 1);
  SelectedIndex = std::max(SelectedIndex, 0);

  if (CurrentTopIndex >= List.Count()) {
    CurrentTopIndex = std::max(CurrentTopIndex - 1, 0);
    if (IsScrollActive) {
      ScrollGadget.Step(1);
    }
  }
}

int ListClass::Action(unsigned flags, KeyNumType& key) {
  if ((flags & LEFTRELEASE) != 0) {
    key = KN_NONE;
    flags &= ~LEFTRELEASE;
    ControlClass::Action(flags, key);
    return true;
  }

  if ((flags & KEYBOARD) != 0) {
    if (key == KN_UP) {
      Step_Selected_Index(-1);
      key = KN_NONE;
    } else if (key == KN_DOWN) {
      Step_Selected_Index(1);
      key = KN_NONE;
    } else {
      flags &= ~KEYBOARD;
    }
  } else if (List.Count() > 0) {
    const int relative = Get_Mouse_Y() - (Y + 1);
    int index = relative / std::max(LineHeight, 1);
    index = ClampIndex(CurrentTopIndex + index, 0, List.Count() - 1);
    SelectedIndex = index;
  }

  return ControlClass::Action(flags, key);
}

int ListClass::Draw_Me(int forced) {
  if (!GadgetClass::Draw_Me(forced)) {
    return false;
  }

  if (LogicPage == &SeenBuff) {
    Conditional_Hide_Mouse(X, Y, X + Width, Y + Height);
  }

  Draw_Box(X, Y, Width, Height, BOXSTYLE_GREEN_BOX, true);

  for (int index = 0; index < LineCount; ++index) {
    const int line = CurrentTopIndex + index;
    if (line >= List.Count()) {
      break;
    }
    Draw_Entry(line, X + 1, Y + (LineHeight * index) + 1, Width - 2,
               line == SelectedIndex);
  }

  if (LogicPage == &SeenBuff) {
    Conditional_Show_Mouse();
  }

  return true;
}

void ListClass::Bump(int up) {
  if (!IsScrollActive) {
    return;
  }
  if (ScrollGadget.Step(up)) {
    CurrentTopIndex = ScrollGadget.Get_Value();
    Flag_To_Redraw();
  }
}

void ListClass::Step(int up) {
  if (!IsScrollActive) {
    return;
  }
  if (ScrollGadget.Step(up)) {
    CurrentTopIndex = ScrollGadget.Get_Value();
    Flag_To_Redraw();
  }
}

char const* ListClass::Get_Item(int index) const {
  if (List.Count() == 0) {
    return nullptr;
  }
  const int clamped = ClampIndex(index, 0, List.Count() - 1);
  return List[clamped];
}

char const* ListClass::Current_Item() {
  if (List.Count() == 0) {
    return nullptr;
  }
  const int clamped = ClampIndex(SelectedIndex, 0, List.Count() - 1);
  return List[clamped];
}

int ListClass::Current_Index() {
  return SelectedIndex;
}

void ListClass::Peer_To_Peer(unsigned flags, KeyNumType&, ControlClass& whom) {
  if ((flags & LEFTRELEASE) != 0) {
    if (&whom == &UpGadget) {
      Step(true);
    } else if (&whom == &DownGadget) {
      Step(false);
    }
  }

  if (&whom == &ScrollGadget) {
    Set_View_Index(ScrollGadget.Get_Value());
  }
}

int ListClass::Set_View_Index(int index) {
  if (List.Count() == 0) {
    CurrentTopIndex = 0;
    return false;
  }

  const int max_top = std::max(0, List.Count() - LineCount);
  const int clamped = ClampIndex(index, 0, max_top);
  if (clamped == CurrentTopIndex) {
    return false;
  }

  CurrentTopIndex = clamped;
  Flag_To_Redraw();
  if (IsScrollActive) {
    ScrollGadget.Set_Value(CurrentTopIndex);
  }
  return true;
}

int ListClass::Add_Scroll_Bar() {
  if (IsScrollActive) {
    return false;
  }
  IsScrollActive = true;
  Width -= ScrollGadget.Width;
  Flag_To_Redraw();

  UpGadget.Make_Peer(*this);
  DownGadget.Make_Peer(*this);
  ScrollGadget.Make_Peer(*this);

  UpGadget.Add(*this);
  DownGadget.Add(*this);
  ScrollGadget.Add(*this);

  UpGadget.Flag_To_Redraw();
  DownGadget.Flag_To_Redraw();
  ScrollGadget.Flag_To_Redraw();

  ScrollGadget.Set_Maximum(List.Count());
  ScrollGadget.Set_Thumb_Size(LineCount);
  ScrollGadget.Set_Value(CurrentTopIndex);
  return true;
}

int ListClass::Remove_Scroll_Bar() {
  if (!IsScrollActive) {
    return false;
  }
  IsScrollActive = false;
  Width += ScrollGadget.Width;
  ScrollGadget.Remove();
  UpGadget.Remove();
  DownGadget.Remove();
  Flag_To_Redraw();
  return true;
}

void ListClass::Set_Tabs(int const* tabs) {
  Tabs = tabs;
}

void ListClass::Draw_Entry(int index, int x, int y, int width, int selected) {
  if (TextFlags & TPF_6PT_GRAD) {
    TextPrintType flags = TextFlags;
    if (selected) {
      flags = static_cast<TextPrintType>(flags | TPF_BRIGHT_COLOR);
      LogicPage->Fill_Rect(x, y, x + width - 1, y + LineHeight - 1, CC_GREEN_SHADOW);
    } else if ((flags & TPF_USE_GRAD_PAL) == 0) {
      flags = static_cast<TextPrintType>(flags | TPF_MEDIUM_COLOR);
    }
    Conquer_Clip_Text_Print(List[index], x, y, CC_GREEN, TBLACK, flags, width, Tabs);
  } else {
    const int color = selected ? BLUE : WHITE;
    Conquer_Clip_Text_Print(List[index], x, y, color, TBLACK, TextFlags, width, Tabs);
  }
}

LinkClass& ListClass::Add(LinkClass& list) {
  if (IsScrollActive) {
    ScrollGadget.Add(list);
    DownGadget.Add(list);
    UpGadget.Add(list);
  }
  return ControlClass::Add(list);
}

LinkClass& ListClass::Add_Head(LinkClass& list) {
  if (IsScrollActive) {
    ScrollGadget.Add_Head(list);
    DownGadget.Add_Head(list);
    UpGadget.Add_Head(list);
  }
  return ControlClass::Add_Head(list);
}

LinkClass& ListClass::Add_Tail(LinkClass& list) {
  if (IsScrollActive) {
    ScrollGadget.Add_Tail(list);
    DownGadget.Add_Tail(list);
    UpGadget.Add_Tail(list);
  }
  return ControlClass::Add_Tail(list);
}

GadgetClass* ListClass::Remove() {
  if (IsScrollActive) {
    ScrollGadget.Remove();
    DownGadget.Remove();
    UpGadget.Remove();
  }
  return ControlClass::Remove();
}

void ListClass::Set_Selected_Index(int index) {
  if (List.Count() == 0) {
    SelectedIndex = 0;
    return;
  }
  const int clamped = ClampIndex(index, 0, List.Count() - 1);
  if (clamped == SelectedIndex) {
    return;
  }

  SelectedIndex = clamped;
  Flag_To_Redraw();

  if (SelectedIndex < CurrentTopIndex) {
    Set_View_Index(SelectedIndex);
  } else if (SelectedIndex >= CurrentTopIndex + LineCount) {
    Set_View_Index(SelectedIndex - (LineCount - 1));
  }
}

int ListClass::Step_Selected_Index(int step) {
  const int old_index = SelectedIndex;
  Set_Selected_Index(old_index + step);
  return old_index;
}
