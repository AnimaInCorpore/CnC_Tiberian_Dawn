#include <cassert>
#include "legacy/function.h"
#include "legacy/dial8.h"
#include "legacy/door.h"

int main() {
    // Dial8 basic sanity
    Dial8Class dial(1, 0, 0, 20, 20, static_cast<DirType>(0));
    assert(dial.Get_Direction() == static_cast<DirType>(0));

    dial.Set_Direction(static_cast<DirType>(64));
    assert(dial.Get_Direction() == static_cast<DirType>(64));

    // Door basic sanity
    DoorClass door;
    assert(door.Is_Door_Closed());
    assert(door.Door_Stage() == 0);

    bool opened = door.Open_Door(1, 4);
    assert(opened);
    assert(door.Is_Door_Opening());

    return 0;
}
