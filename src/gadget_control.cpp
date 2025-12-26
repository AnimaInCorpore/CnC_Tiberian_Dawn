#include "legacy/control.h"
#include "legacy/externs.h"
#include "legacy/gadget.h"
#include "legacy/jshell.h"
#include "legacy/wwlib32.h"

GadgetClass* GadgetClass::StuckOn = nullptr;
GadgetClass* GadgetClass::LastList = nullptr;
GadgetClass* GadgetClass::Focused = nullptr;

GadgetClass::GadgetClass(int x, int y, int w, int h, unsigned flags, int sticky)
    : X(x),
      Y(y),
      Width(w),
      Height(h),
      IsSticky(sticky ? 1u : 0u),
      IsDisabled(0),
      Flags(flags) {
  IsToRepaint = false;
  if (IsSticky) {
    Flags |= LEFTPRESS | LEFTRELEASE;
  }
}

GadgetClass::~GadgetClass() = default;

KeyNumType GadgetClass::Input(void) {
  int mousex;
  int mousey;
  KeyNumType key = KN_NONE;
  unsigned flags = 0;
  bool forced = false;

  if (LastList != this) {
    LastList = this;
    forced = true;
    StuckOn = nullptr;
    Focused = nullptr;
  }

  key = Keyboard::Check();
  if (key) {
    key = Keyboard::Get();
  }

  if (((key & 0x10FF) == KN_LMOUSE) || ((key & 0x10FF) == KN_RMOUSE)) {
    mousex = _Kbd->MouseQX;
    mousey = _Kbd->MouseQY;
  } else {
    mousex = Get_Mouse_X();
    mousey = Get_Mouse_Y();
  }

  if (key) {
    if (key == KN_LMOUSE) {
      flags |= LEFTPRESS;
    }
    if (key == KN_RMOUSE) {
      flags |= RIGHTPRESS;
    }
    if (key == (KN_LMOUSE | KN_RLSE_BIT)) {
      flags |= LEFTRELEASE;
    }
    if (key == (KN_RMOUSE | KN_RLSE_BIT)) {
      flags |= RIGHTRELEASE;
    }
  }

  if (key && !flags) {
    flags |= KEYBOARD;
  }

  if (!key) {
    if (Keyboard::Down(KN_LMOUSE)) {
      flags |= LEFTHELD;
    } else {
      flags |= LEFTUP;
    }
    if (Keyboard::Down(KN_RMOUSE)) {
      flags |= RIGHTHELD;
    } else {
      flags |= RIGHTUP;
    }
  }

  if (StuckOn) {
    StuckOn->Draw_Me(false);
    StuckOn->Clicked_On(key, flags, mousex, mousey);
    if (StuckOn) {
      StuckOn->Draw_Me(false);
    }
  } else if (Focused && (flags & KEYBOARD)) {
    Focused->Draw_Me(false);
    Focused->Clicked_On(key, flags, mousex, mousey);
    if (Focused) {
      Focused->Draw_Me(false);
    }
  } else {
    GadgetClass* next_button = this;
    while (next_button != nullptr) {
      next_button->Draw_Me(forced);

      if (!next_button->IsDisabled) {
        if (next_button->Clicked_On(key, flags, mousex, mousey)) {
          next_button->Draw_Me(false);
          break;
        }
      }

      next_button = next_button->Get_Next();
    }
  }
  return key;
}

void GadgetClass::Draw_All(bool forced) {
  GadgetClass* gadget = this;
  while (gadget) {
    gadget->Draw_Me(forced);
    gadget = gadget->Get_Next();
  }
}

void GadgetClass::Delete_List(void) {
  GadgetClass* g = this;
  while (g->Get_Prev()) {
    g = g->Get_Prev();
  }

  while (g) {
    g->Clear_Focus();
    GadgetClass* temp = g;
    g = g->Get_Next();
    delete temp;
  }
}

ControlClass* GadgetClass::Extract_Gadget(unsigned id) {
  GadgetClass* g = this;
  if (id == 0) {
    return nullptr;
  }

  while (g) {
    if (g->Get_ID() == id) {
      return static_cast<ControlClass*>(g);
    }
    g = g->Get_Next();
  }
  return nullptr;
}

GadgetClass* GadgetClass::Remove(void) {
  Clear_Focus();
  return static_cast<GadgetClass*>(LinkClass::Remove());
}

GadgetClass* GadgetClass::Get_Next(void) const {
  return static_cast<GadgetClass*>(LinkClass::Get_Next());
}

GadgetClass* GadgetClass::Get_Prev(void) const {
  return static_cast<GadgetClass*>(LinkClass::Get_Prev());
}

void GadgetClass::Disable(void) {
  IsDisabled = true;
  IsToRepaint = true;
  Clear_Focus();
}

void GadgetClass::Enable(void) {
  IsDisabled = false;
  IsToRepaint = true;
  Clear_Focus();
}

void GadgetClass::Flag_To_Redraw(void) { IsToRepaint = true; }

void GadgetClass::Set_Focus(void) {
  if (Focused) {
    Focused->Flag_To_Redraw();
    Focused->Clear_Focus();
  }
  Flags |= KEYBOARD;
  Focused = this;
}

void GadgetClass::Clear_Focus(void) {
  if (Focused == this) {
    Flags &= ~KEYBOARD;
    Focused = nullptr;
  }
}

bool GadgetClass::Has_Focus(void) { return Focused == this; }

int GadgetClass::Is_List_To_Redraw(void) {
  GadgetClass* gadget = this;
  while (gadget) {
    if (gadget->IsToRepaint) {
      return true;
    }
    gadget = gadget->Get_Next();
  }
  return false;
}

int GadgetClass::Draw_Me(int forced) {
  if (forced || IsToRepaint) {
    IsToRepaint = false;
    return true;
  }
  return false;
}

void GadgetClass::Sticky_Process(unsigned flags) {
  if (IsSticky && (flags & LEFTPRESS)) {
    StuckOn = this;
  }
  if (StuckOn == this && (flags & LEFTRELEASE)) {
    StuckOn = nullptr;
  }
}

int GadgetClass::Action(unsigned flags, KeyNumType&) {
  if (flags) {
    IsToRepaint = true;
    Sticky_Process(flags);
    return true;
  }
  return false;
}

int GadgetClass::Clicked_On(KeyNumType& key, unsigned flags, int mousex, int mousey) {
  flags &= Flags;

  if (this == StuckOn || (flags & KEYBOARD) ||
      (flags && static_cast<unsigned>(mousex - X) < static_cast<unsigned>(Width) &&
       static_cast<unsigned>(mousey - Y) < static_cast<unsigned>(Height))) {
    return Action(flags, key);
  }
  return false;
}

