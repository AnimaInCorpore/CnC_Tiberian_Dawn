#include "legacy/function.h"
#include "legacy/ccfile.h"

bool Expansion_Present(void) {
  CCFileClass file("EXPAND.DAT");
  return file.Is_Available();
}
