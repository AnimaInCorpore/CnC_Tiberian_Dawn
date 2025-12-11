#include "legacy/cargo.h"

#include "legacy/foot.h"
#include "legacy/function.h"
#include "legacy/object.h"
#include "legacy/target.h"

#ifdef CHEAT_KEYS
#include "legacy/monoc.h"
#endif

#include <cstdint>

#ifdef CHEAT_KEYS
void CargoClass::Debug_Dump(MonoClass* mono) const {
  if (!mono || !How_Many()) {
    return;
  }

  mono->Set_Cursor(63, 3);
  mono->Printf("(%d)%04X", How_Many(), Attached_Object());
}
#endif

void CargoClass::Attach(FootClass* object) {
  if (!object) return;

  object->Limbo();

  ObjectClass* tail = object->Next;
  while (tail && tail->Next) {
    tail = tail->Next;
  }
  if (tail) {
    tail->Next = CargoHold;
  } else {
    object->Next = CargoHold;
  }

  CargoHold = object;

  Quantity = 0;
  FootClass* walker = CargoHold;
  while (walker) {
    ++Quantity;
    walker = static_cast<FootClass*>(walker->Next);
  }
}

FootClass* CargoClass::Detach_Object() {
  FootClass* unit = Attached_Object();
  if (unit) {
    CargoHold = static_cast<FootClass*>(unit->Next);
    unit->Next = nullptr;
    if (Quantity > 0) {
      --Quantity;
    }
  }
  return unit;
}

FootClass* CargoClass::Attached_Object() const {
  return Is_Something_Attached() ? CargoHold : nullptr;
}

void CargoClass::Code_Pointers() {
  if (CargoHold) {
    CargoHold = reinterpret_cast<FootClass*>(
        static_cast<std::uintptr_t>(CargoHold->As_Target()));
  }
}

void CargoClass::Decode_Pointers() {
  if (CargoHold) {
    CargoHold = static_cast<FootClass*>(As_Techno(
        static_cast<TARGET>(reinterpret_cast<std::uintptr_t>(CargoHold))));
    Check_Ptr(CargoHold, __FILE__, __LINE__);
  }
}
