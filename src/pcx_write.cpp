#include "legacy/function.h"

#include <cstdint>
#include <cstring>

namespace {

struct PcxHeader {
  std::uint8_t manufacturer;
  std::uint8_t version;
  std::uint8_t encoding;
  std::uint8_t bits_per_pixel;
  std::uint16_t xmin;
  std::uint16_t ymin;
  std::uint16_t xmax;
  std::uint16_t ymax;
  std::uint16_t hres;
  std::uint16_t vres;
  std::uint8_t palette16[48];
  std::uint8_t reserved;
  std::uint8_t color_planes;
  std::uint16_t bytes_per_line;
  std::uint16_t palette_type;
  std::uint16_t hscreen;
  std::uint16_t vscreen;
  std::uint8_t filler[54];
};

void Write_Rle_Run(CCFileClass& file, std::uint8_t value, std::uint8_t count) {
  if (count > 1 || (value & 0xC0) == 0xC0) {
    const std::uint8_t marker = static_cast<std::uint8_t>(0xC0 | count);
    file.Write(&marker, 1);
  }
  file.Write(&value, 1);
}

}  // namespace

void Write_PCX_File(char const* filename, GraphicBufferClass const& page, unsigned char const* palette) {
  if (!filename) return;

  CCFileClass file(filename);
  if (!file.Open(WRITE)) {
    return;
  }

  const int width = page.Get_Width();
  const int height = page.Get_Height();

  PcxHeader header{};
  header.manufacturer = 0x0A;
  header.version = 5;
  header.encoding = 1;
  header.bits_per_pixel = 8;
  header.xmin = 0;
  header.ymin = 0;
  header.xmax = static_cast<std::uint16_t>(width - 1);
  header.ymax = static_cast<std::uint16_t>(height - 1);
  header.hres = static_cast<std::uint16_t>(width);
  header.vres = static_cast<std::uint16_t>(height);
  header.color_planes = 1;
  header.bytes_per_line = static_cast<std::uint16_t>(width);
  header.palette_type = 1;

  file.Write(&header, sizeof(header));

  const unsigned char* buffer = page.Get_Buffer();
  for (int y = 0; y < height; ++y) {
    const unsigned char* row = buffer + (y * width);
    int x = 0;
    while (x < width) {
      std::uint8_t value = row[x];
      std::uint8_t count = 1;
      while (x + count < width && row[x + count] == value && count < 63) {
        ++count;
      }
      Write_Rle_Run(file, value, count);
      x += count;
    }
  }

  if (palette) {
    const std::uint8_t marker = 0x0C;
    file.Write(&marker, 1);
    file.Write(palette, 256 * 3);
  }

  file.Close();
}
