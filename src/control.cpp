#include "function.h"
#include "control.h"

ControlClass::~ControlClass() = default;

ControlClass::ControlClass(unsigned id, int x, int y, int w, int h, unsigned flags, int sticky)
    : GadgetClass(x, y, w, h, flags, sticky), ID(id), Peer(nullptr) {}

int ControlClass::Action(unsigned flags, KeyNumType& key) {
  if (Peer) {
    Peer->Peer_To_Peer(flags, key, *this);
  }

  if (flags) {
    if (ID) {
      key = static_cast<KeyNumType>(ID | KN_BUTTON);
    } else {
      key = KN_NONE;
    }
  }

  return GadgetClass::Action(flags, key);
}

void ControlClass::Make_Peer(GadgetClass& gadget) { Peer = &gadget; }

unsigned ControlClass::Get_ID(void) const { return ID; }

int ControlClass::Draw_Me(int forced) {
  if (Peer) {
    Peer->Draw_Me();
  }
  return GadgetClass::Draw_Me(forced);
}
