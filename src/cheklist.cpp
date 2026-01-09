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
    char* item = Item_At(index);
    if (!item) return;
    item[0] = checked ? CHECK_CHAR : UNCHECK_CHAR;
}

int CheckListClass::Is_Checked(int index) const {
    char const* item = Item_At(index);
    if (!item) return 0;
    return item[0] == CHECK_CHAR;
}

int CheckListClass::Action(unsigned flags, KeyNumType& key) {
    if (IsReadOnly) return 0;

    int rc = ListClass::Action(flags, key);

    if (flags & LEFTPRESS) {
        Check_Item(SelectedIndex, Is_Checked(SelectedIndex) ? 0 : 1);
    }

    return rc;
}

