#include "legacy/function.h"

#include <array>
#include <cstring>

void* Small_Icon(void const* iconptr, int iconnum) {
  static std::array<unsigned char, 9> icon = {0, 0, 0, 0, 0, 0, 0, 0, 0};

  if (!iconptr) return icon.data();

  const auto* base = static_cast<const unsigned char*>(iconptr);

  long map_offset = 0;
  std::memcpy(&map_offset, base + 28, sizeof(map_offset));
  const unsigned char* map = (map_offset > 0) ? (base + map_offset) : nullptr;

  int actual_icon = iconnum;
  if (map) {
    actual_icon = static_cast<int>(map[iconnum]);
    if (actual_icon == 0xFF) return icon.data();
  }

  long data_offset = 0;
  std::memcpy(&data_offset, base + 12, sizeof(data_offset));
  if (data_offset <= 0) return icon.data();

  const int icon_w = ICON_PIXEL_W;
  const int icon_h = ICON_PIXEL_H;
  const long icon_stride = static_cast<long>(icon_w) * icon_h;
  const unsigned char* data = base + data_offset + static_cast<long>(actual_icon) * icon_stride;

  const std::array<int, 9> offsets = {
      4 + 4 * icon_w,
      12 + 4 * icon_w,
      20 + 4 * icon_w,
      4 + 12 * icon_w,
      12 + 12 * icon_w,
      20 + 12 * icon_w,
      4 + 20 * icon_w,
      12 + 20 * icon_w,
      20 + 20 * icon_w,
  };

  for (std::size_t index = 0; index < icon.size(); ++index) {
    const int offset = offsets[index];
    if (offset < 0 || offset >= icon_w * icon_h) {
      icon[index] = 0;
      continue;
    }
    icon[index] = data[offset];
  }

  return icon.data();
}

