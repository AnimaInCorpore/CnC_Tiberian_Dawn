#include "legacy/gadget.h"
#include "legacy/control.h"

GadgetClass* GadgetClass::StuckOn = nullptr;
GadgetClass* GadgetClass::LastList = nullptr;
GadgetClass* GadgetClass::Focused = nullptr;

GadgetClass::GadgetClass(int x, int y, int w, int h, unsigned, int)
    : X(x), Y(y), Width(w), Height(h) {}

GadgetClass::~GadgetClass() = default;

KeyNumType GadgetClass::Input(void) { return 0; }

void GadgetClass::Draw_All(bool) {}

void GadgetClass::Delete_List(void) {}

ControlClass* GadgetClass::Extract_Gadget(unsigned) { return nullptr; }

GadgetClass* GadgetClass::Remove(void) { return nullptr; }

GadgetClass* GadgetClass::Get_Next(void) const { return nullptr; }

GadgetClass* GadgetClass::Get_Prev(void) const { return nullptr; }

void GadgetClass::Disable(void) {}

void GadgetClass::Enable(void) {}

void GadgetClass::Flag_To_Redraw(void) {}

void GadgetClass::Set_Focus(void) { Focused = this; }

void GadgetClass::Clear_Focus(void) {
  if (Focused == this) {
    Focused = nullptr;
  }
}

bool GadgetClass::Has_Focus(void) { return Focused == this; }

int GadgetClass::Is_List_To_Redraw(void) { return 0; }

int GadgetClass::Draw_Me(int) { return 0; }

void GadgetClass::Sticky_Process(unsigned) {}

int GadgetClass::Action(unsigned, KeyNumType&) { return 0; }

ControlClass::~ControlClass() = default;

ControlClass::ControlClass(unsigned id, int x, int y, int w, int h,
                           unsigned flags, int sticky)
    : GadgetClass(x, y, w, h, flags, sticky), ID(id), Peer(nullptr) {}

void ControlClass::Make_Peer(GadgetClass& gadget) { Peer = &gadget; }

int ControlClass::Draw_Me(int forced) { return GadgetClass::Draw_Me(forced); }

unsigned ControlClass::Get_ID(void) const { return ID; }

int ControlClass::Action(unsigned flags, KeyNumType& key) {
  key = static_cast<KeyNumType>(ID);
  return flags ? 1 : 0;
}
