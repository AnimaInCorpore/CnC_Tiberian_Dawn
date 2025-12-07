#include "legacy/function.h"

#include <algorithm>
#include <cctype>
#include <string>

void Add_CRC(unsigned long* crc, unsigned long val) {
  if (!crc) return;

  const int hibit = (*crc & 0x80000000) ? 1 : 0;
  *crc <<= 1;
  *crc += val;
  *crc += hibit;
}

unsigned long Compute_Name_CRC(char* name) {
  if (!name) return 0;

  std::string buffer{name};
  std::transform(buffer.begin(), buffer.end(), buffer.begin(),
                 [](unsigned char c) { return static_cast<char>(std::toupper(c)); });

  unsigned long crc = 0;
  for (unsigned char c : buffer) {
    Add_CRC(&crc, static_cast<unsigned long>(c));
  }
  return crc;
}
