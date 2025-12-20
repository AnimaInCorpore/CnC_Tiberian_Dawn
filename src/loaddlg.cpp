#include "legacy/loaddlg.h"
#include "legacy/list.h"

LoadOptionsClass::LoadOptionsClass(LoadStyleType style) : Style(style) {
  Files.Clear();
}

LoadOptionsClass::~LoadOptionsClass() {
  for (int i = 0; i < Files.Count(); i++) {
    delete Files[i];
  }
  Files.Clear();
}

int LoadOptionsClass::Process(void) {
  return false;
}

void LoadOptionsClass::Clear_List(ListClass*) {}
void LoadOptionsClass::Fill_List(ListClass*) {}
int LoadOptionsClass::Num_From_Ext(char*) { return 0; }
int LoadOptionsClass::Compare(const void*, const void*) { return 0; }
