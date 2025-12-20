#include "legacy/function.h"

unsigned long Calculate_CRC(void const* buffer, int length) {
  if (!buffer || length <= 0) return 0;

  unsigned long crc = 0;
  auto* data = static_cast<unsigned char const*>(buffer);
  for (int i = 0; i < length; ++i) {
    Add_CRC(&crc, static_cast<unsigned long>(data[i]));
  }
  return crc;
}
