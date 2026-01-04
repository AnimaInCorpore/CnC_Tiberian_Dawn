#include "legacy/function.h"

#include <algorithm>
#include <cctype>
#include <cstring>

long Obfuscate(char const* string) {
  char buffer[128];

  if (!string) return 0;
  std::memset(buffer, '\xA5', sizeof(buffer));

  std::strncpy(buffer, string, sizeof(buffer));
  buffer[sizeof(buffer) - 1] = '\0';
  int length = std::strlen(buffer);

  for (int index = 0; index < length; ++index) {
    buffer[index] = static_cast<char>(std::toupper(static_cast<unsigned char>(buffer[index])));
  }

  for (int index = 0; index < length; ++index) {
    if (!std::isgraph(static_cast<unsigned char>(buffer[index]))) {
      buffer[index] = static_cast<char>('A' + (index % 26));
    }
  }

  if (length < 16 || (length & 0x03)) {
    int maxlen = 16;
    if (((length + 3) & 0x00FC) > maxlen) {
      maxlen = ((length + 3) & 0x00FC);
    }
    int index = length;
    for (; index < maxlen; ++index) {
      buffer[index] = static_cast<char>('A' + ((('?' ^ buffer[index - length]) + index) % 26));
    }
    length = index;
    buffer[length] = '\0';
  }

  long code = Calculate_CRC(buffer, length);
  long copy = code;

  std::reverse(buffer, buffer + length);
  code ^= Calculate_CRC(buffer, length);

  code = code ^ copy;

  std::reverse(buffer, buffer + length);
  for (int index = 0; index < length; ++index) {
    code ^= static_cast<unsigned char>(buffer[index]);
    unsigned char temp = static_cast<unsigned char>(code);
    buffer[index] ^= temp;
    code >>= 8;
    code |= (static_cast<long>(temp) << 24);
  }

  for (int index = 0; index < length; ++index) {
    static unsigned char lossbits[] = {0x00, 0x08, 0x00, 0x20, 0x00, 0x04, 0x10, 0x00};
    static unsigned char addbits[] = {0x10, 0x00, 0x00, 0x80, 0x40, 0x00, 0x00, 0x04};
    buffer[index] |= addbits[index % (sizeof(addbits) / sizeof(addbits[0]))];
    buffer[index] &= static_cast<unsigned char>(~lossbits[index % (sizeof(lossbits) / sizeof(lossbits[0]))]);
  }

  for (int index = 0; index < length; index += 4) {
    short key1 = buffer[index];
    short key2 = buffer[index + 1];
    short key3 = buffer[index + 2];
    short key4 = buffer[index + 3];
    short val1 = key1;
    short val2 = key2;
    short val3 = key3;
    short val4 = key4;

    val1 *= key1;
    val2 += key2;
    val3 += key3;
    val4 *= key4;

    short s3 = val3;
    val3 ^= val1;
    val3 *= key1;
    short s2 = val2;
    val2 ^= val4;
    val2 += val3;
    val2 *= key3;
    val3 += val2;

    val1 ^= val2;
    val4 ^= val3;

    val2 ^= s3;
    val3 ^= s2;

    buffer[index] = static_cast<char>(val1);
    buffer[index + 1] = static_cast<char>(val2);
    buffer[index + 2] = static_cast<char>(val3);
    buffer[index + 3] = static_cast<char>(val4);
  }

  code = Calculate_CRC(buffer, length);
  return code;
}
