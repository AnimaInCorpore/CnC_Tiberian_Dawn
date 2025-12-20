#include "legacy/function.h"

static bool UseBigShapeBuffer = false;
static bool OriginalUseBigShapeBuffer = false;
static int LastFrameLength = 0;

void Disable_Uncompressed_Shapes(void) {
  UseBigShapeBuffer = false;
}

void Enable_Uncompressed_Shapes(void) {
  UseBigShapeBuffer = OriginalUseBigShapeBuffer;
}

void* Get_Shape_Header_Data(void* ptr) {
  return ptr;
}

int Get_Last_Frame_Length(void) {
  return LastFrameLength;
}

unsigned long Build_Frame(void const*, unsigned short, void* buffptr) {
  LastFrameLength = 0;
  return reinterpret_cast<unsigned long>(buffptr);
}
