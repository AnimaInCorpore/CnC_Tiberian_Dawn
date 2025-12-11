#include "function.h"
#include "cheklist.h"

CheckListClass::CheckListClass(int id, int x, int y, int w, int h, TextPrintType flags,
                               void const* up, void const* down)
    : ListClass(id, x, y, w, h, flags, up, down), IsReadOnly(false) {}

void CheckListClass::Check_Item(int index, int checked) {
  if (index < 0 || index >= List.Count()) {
    return;
  }

  char const* entry = List[index];
  if (!entry) {
    return;
  }

  char* writable = const_cast<char*>(entry);
  writable[0] = checked ? CHECK_CHAR : UNCHECK_CHAR;
}

int CheckListClass::Is_Checked(int index) const {
  if (index < 0 || index >= List.Count()) {
    return false;
  }

  char const* entry = List[index];
  return entry != nullptr && entry[0] == CHECK_CHAR;
}

int CheckListClass::Action(unsigned flags, KeyNumType& key) {
  if (IsReadOnly) {
    return false;
  }

  const int handled = ListClass::Action(flags, key);
  if ((flags & LEFTPRESS) && SelectedIndex >= 0 && SelectedIndex < List.Count()) {
    const bool checked = Is_Checked(SelectedIndex);
    Check_Item(SelectedIndex, checked ? 0 : 1);
  }
  return handled;
}
