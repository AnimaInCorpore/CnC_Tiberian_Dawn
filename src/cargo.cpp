#include "cargo.h"

#include "legacy_compat.h"

#ifdef CHEAT_KEYS
void CargoClass::Debug_Dump(MonoClass* mono) const {
    if (How_Many()) {
        mono->Set_Cursor(63, 3);
        mono->Printf("(%d)%04X", How_Many(), Attached_Object());
    }
}
#endif

void CargoClass::Attach(FootClass* object) {
    if (!object) return;

    object->Limbo();

    ObjectClass* tail = object->Next;
    while (tail) {
        if (!tail->Next) break;
        tail = tail->Next;
    }

    if (tail) {
        tail->Next = CargoHold;
    } else {
        object->Next = CargoHold;
    }

    CargoHold = object;
    Quantity = 0;
    object = CargoHold;
    while (object) {
        ++Quantity;
        object = static_cast<FootClass*>(object->Next);
    }
}

FootClass* CargoClass::Detach_Object(void) {
    FootClass* unit = Attached_Object();

    if (unit) {
        CargoHold = static_cast<FootClass*>(unit->Next);
        unit->Next = 0;
        --Quantity;
    }
    return unit;
}

FootClass* CargoClass::Attached_Object(void) const {
    if (Is_Something_Attached()) {
        return CargoHold;
    }
    return 0;
}

