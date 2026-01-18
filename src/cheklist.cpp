#include "cheklist.h"

CheckListClass::CheckListClass(int id,
                               int x,
                               int y,
                               int w,
                               int h,
                               TextPrintType flags,
                               void const* up,
                               void const* down)
    : ListClass(id, x, y, w, h, flags, up, down), IsReadOnly(false) {}

void CheckListClass::Check_Item(int index, int checked) {
    if (index < 0 || index >= List.Count()) return;
    if (!List[index]) return;
    List[index][0] = checked ? CHECK_CHAR : UNCHECK_CHAR;
}

int CheckListClass::Is_Checked(int index) const {
    if (index < 0 || index >= List.Count()) return 0;
    if (!List[index]) return 0;
    return List[index][0] == CHECK_CHAR;
}

int CheckListClass::Action(unsigned flags, KeyNumType& key) {
    if (IsReadOnly) return false;

    int rc = ListClass::Action(flags, key);

    if (flags & LEFTPRESS) {
        if (Is_Checked(SelectedIndex)) {
            Check_Item(SelectedIndex, 0);
        } else {
            Check_Item(SelectedIndex, 1);
        }
    }

    return rc;
}
