#include "legacy/function.h"

void Bit_It_In_Scale(int x, int y, int w, int h, GraphicBufferClass* src, GraphicBufferClass* dest,
                     GraphicViewPortClass* seen, int, int) {
  if (!src || !dest) return;
  src->Blit(*dest, x, y, 0, 0, w, h);
  (void)seen;
}
