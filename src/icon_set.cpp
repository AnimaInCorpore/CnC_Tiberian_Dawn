#include "legacy/function.h"

#include <cstring>

void* Get_Icon_Set_Map(void const* iconset) {
  if (!iconset) return nullptr;
  const auto* base = static_cast<const unsigned char*>(iconset);
  long map_offset = 0;
  std::memcpy(&map_offset, base + 28, sizeof(map_offset));
  if (map_offset <= 0) return nullptr;
  return const_cast<unsigned char*>(base + map_offset);
}

void Register_Icon_Set(void*, bool) {
  // SDL rendering does not require DirectDraw-style icon caching.
}

void Invalidate_Cached_Icons() {
  // The original Win95 build invalidated cached icon surfaces between scenarios.
  // The SDL renderer rebuilds icon state on demand, so nothing is required here.
}
