/*
**	Simple text rendering helpers for the SDL port.
*/

#include "legacy/function.h"
#include "legacy/externs.h"
#include "legacy/wwlib32.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

int FontHeight = 8;
int FontYSpacing = 1;
int FontXSpacing = 0;

namespace {

constexpr int kFontWidth = 8;
constexpr int kFontHeight = 8;

struct ParsedFont {
  const unsigned char* base = nullptr;
  const unsigned char* data_base = nullptr;
  const std::uint16_t* offsets = nullptr;
  const unsigned char* widths = nullptr;
  const unsigned char* heights = nullptr;  // two bytes per character: top blank, data height
  int max_height = kFontHeight;
  int max_width = kFontWidth;
};

ParsedFont g_current_font{};
bool g_current_font_valid = false;

// Public-domain 8x8 bitmap font (font8x8_basic).
static const unsigned char kFont8x8[128][8] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+0000 (nul)
    {0x7E, 0x81, 0xA5, 0x81, 0xBD, 0x99, 0x81, 0x7E},  // U+0001
    {0x7E, 0xFF, 0xDB, 0xFF, 0xC3, 0xE7, 0xFF, 0x7E},  // U+0002
    {0x6C, 0xFE, 0xFE, 0xFE, 0x7C, 0x38, 0x10, 0x00},  // U+0003
    {0x10, 0x38, 0x7C, 0xFE, 0x7C, 0x38, 0x10, 0x00},  // U+0004
    {0x38, 0x7C, 0x38, 0xFE, 0xFE, 0x92, 0x10, 0x7C},  // U+0005
    {0x10, 0x38, 0x7C, 0xFE, 0xFE, 0x7C, 0x10, 0x38},  // U+0006
    {0x00, 0x00, 0x18, 0x3C, 0x3C, 0x18, 0x00, 0x00},  // U+0007
    {0xFF, 0xFF, 0xE7, 0xC3, 0xC3, 0xE7, 0xFF, 0xFF},  // U+0008
    {0x00, 0x3C, 0x66, 0x42, 0x42, 0x66, 0x3C, 0x00},  // U+0009
    {0xFF, 0xC3, 0x99, 0xBD, 0xBD, 0x99, 0xC3, 0xFF},  // U+000A
    {0x70, 0xF8, 0x88, 0x88, 0xFD, 0x7F, 0x07, 0x0F},  // U+000B
    {0x00, 0x4E, 0x5F, 0xF1, 0xF1, 0x5F, 0x4E, 0x00},  // U+000C
    {0xC0, 0xE0, 0x7F, 0x3F, 0x3F, 0x7F, 0xE0, 0xC0},  // U+000D
    {0x08, 0x1C, 0x3E, 0x7F, 0x3E, 0x1C, 0x08, 0x00},  // U+000E
    {0x08, 0x1C, 0x3E, 0x7F, 0x3E, 0x1C, 0x08, 0x00},  // U+000F
    {0x00, 0x00, 0x18, 0x3C, 0x3C, 0x18, 0x00, 0x00},  // U+0010
    {0xFF, 0xFF, 0xE7, 0xC3, 0xC3, 0xE7, 0xFF, 0xFF},  // U+0011
    {0x0F, 0x07, 0x7F, 0x3F, 0x3F, 0x7F, 0xE0, 0xC0},  // U+0012
    {0x38, 0x7C, 0x3E, 0x7F, 0x7F, 0x3E, 0x7C, 0x38},  // U+0013
    {0x10, 0x10, 0x38, 0x7C, 0xFE, 0x7C, 0x38, 0x7C},  // U+0014
    {0x00, 0x00, 0x00, 0x00, 0x7E, 0x7E, 0x7E, 0x00},  // U+0015
    {0x10, 0x38, 0x7C, 0xFE, 0xFE, 0x10, 0x10, 0x38},  // U+0016
    {0x10, 0x10, 0x10, 0x10, 0xFE, 0xFE, 0xFE, 0x00},  // U+0017
    {0x00, 0x00, 0x18, 0x3C, 0x3C, 0x18, 0x00, 0x00},  // U+0018
    {0xFF, 0xFF, 0xE7, 0xC3, 0xC3, 0xE7, 0xFF, 0xFF},  // U+0019
    {0x0F, 0x07, 0x7F, 0x3F, 0x3F, 0x7F, 0xE0, 0xC0},  // U+001A
    {0x10, 0x38, 0x7C, 0xFE, 0xFE, 0x7C, 0x38, 0x10},  // U+001B
    {0x10, 0x10, 0x38, 0x7C, 0xFE, 0x7C, 0x38, 0x10},  // U+001C
    {0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38},  // U+001D
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+001E
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+001F
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+0020 (space)
    {0x18, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x18, 0x00},  // U+0021 (!)
    {0x6C, 0x6C, 0x48, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+0022 (")
    {0x6C, 0x6C, 0xFE, 0x6C, 0xFE, 0x6C, 0x6C, 0x00},  // U+0023 (#)
    {0x18, 0x3E, 0x60, 0x3C, 0x06, 0x7C, 0x18, 0x00},  // U+0024 ($)
    {0x00, 0xC6, 0xCC, 0x18, 0x30, 0x66, 0xC6, 0x00},  // U+0025 (%)
    {0x38, 0x6C, 0x38, 0x76, 0xDC, 0xCC, 0x76, 0x00},  // U+0026 (&)
    {0x30, 0x30, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+0027 (')
    {0x0C, 0x18, 0x30, 0x30, 0x30, 0x18, 0x0C, 0x00},  // U+0028 (()
    {0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x18, 0x30, 0x00},  // U+0029 ())
    {0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00},  // U+002A (*)
    {0x00, 0x18, 0x18, 0x7E, 0x18, 0x18, 0x00, 0x00},  // U+002B (+)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x30},  // U+002C (,)
    {0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00},  // U+002D (-)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00},  // U+002E (.)
    {0x06, 0x0C, 0x18, 0x30, 0x60, 0xC0, 0x80, 0x00},  // U+002F (/)
    {0x7C, 0xC6, 0xCE, 0xD6, 0xE6, 0xC6, 0x7C, 0x00},  // U+0030 (0)
    {0x18, 0x38, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00},  // U+0031 (1)
    {0x7C, 0xC6, 0x0E, 0x1C, 0x70, 0xC0, 0xFE, 0x00},  // U+0032 (2)
    {0x7C, 0xC6, 0x06, 0x3C, 0x06, 0xC6, 0x7C, 0x00},  // U+0033 (3)
    {0x1C, 0x3C, 0x6C, 0xCC, 0xFE, 0x0C, 0x1E, 0x00},  // U+0034 (4)
    {0xFE, 0xC0, 0xFC, 0x06, 0x06, 0xC6, 0x7C, 0x00},  // U+0035 (5)
    {0x3C, 0x60, 0xC0, 0xFC, 0xC6, 0xC6, 0x7C, 0x00},  // U+0036 (6)
    {0xFE, 0xC6, 0x0C, 0x18, 0x30, 0x30, 0x30, 0x00},  // U+0037 (7)
    {0x7C, 0xC6, 0xC6, 0x7C, 0xC6, 0xC6, 0x7C, 0x00},  // U+0038 (8)
    {0x7C, 0xC6, 0xC6, 0x7E, 0x06, 0x0C, 0x78, 0x00},  // U+0039 (9)
    {0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x00},  // U+003A (:)
    {0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x30},  // U+003B (;)
    {0x0E, 0x1C, 0x38, 0x70, 0x38, 0x1C, 0x0E, 0x00},  // U+003C (<)
    {0x00, 0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00, 0x00},  // U+003D (=)
    {0x70, 0x38, 0x1C, 0x0E, 0x1C, 0x38, 0x70, 0x00},  // U+003E (>)
    {0x7C, 0xC6, 0x0E, 0x1C, 0x18, 0x00, 0x18, 0x00},  // U+003F (?)
    {0x7C, 0xC6, 0xDE, 0xDE, 0xDE, 0xC0, 0x7C, 0x00},  // U+0040 (@)
    {0x38, 0x6C, 0xC6, 0xFE, 0xC6, 0xC6, 0xC6, 0x00},  // U+0041 (A)
    {0xFC, 0x66, 0x66, 0x7C, 0x66, 0x66, 0xFC, 0x00},  // U+0042 (B)
    {0x3C, 0x66, 0xC0, 0xC0, 0xC0, 0x66, 0x3C, 0x00},  // U+0043 (C)
    {0xF8, 0x6C, 0x66, 0x66, 0x66, 0x6C, 0xF8, 0x00},  // U+0044 (D)
    {0xFE, 0x62, 0x68, 0x78, 0x68, 0x62, 0xFE, 0x00},  // U+0045 (E)
    {0xFE, 0x62, 0x68, 0x78, 0x68, 0x60, 0xF0, 0x00},  // U+0046 (F)
    {0x3C, 0x66, 0xC0, 0xC0, 0xCE, 0x66, 0x3A, 0x00},  // U+0047 (G)
    {0xC6, 0xC6, 0xC6, 0xFE, 0xC6, 0xC6, 0xC6, 0x00},  // U+0048 (H)
    {0x3C, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00},  // U+0049 (I)
    {0x1E, 0x0C, 0x0C, 0x0C, 0xCC, 0xCC, 0x78, 0x00},  // U+004A (J)
    {0xE6, 0x66, 0x6C, 0x78, 0x6C, 0x66, 0xE6, 0x00},  // U+004B (K)
    {0xF0, 0x60, 0x60, 0x60, 0x62, 0x66, 0xFE, 0x00},  // U+004C (L)
    {0xC6, 0xEE, 0xFE, 0xFE, 0xD6, 0xC6, 0xC6, 0x00},  // U+004D (M)
    {0xC6, 0xE6, 0xF6, 0xDE, 0xCE, 0xC6, 0xC6, 0x00},  // U+004E (N)
    {0x7C, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x7C, 0x00},  // U+004F (O)
    {0xFC, 0x66, 0x66, 0x7C, 0x60, 0x60, 0xF0, 0x00},  // U+0050 (P)
    {0x7C, 0xC6, 0xC6, 0xC6, 0xD6, 0xCC, 0x7A, 0x00},  // U+0051 (Q)
    {0xFC, 0x66, 0x66, 0x7C, 0x6C, 0x66, 0xE6, 0x00},  // U+0052 (R)
    {0x7C, 0xC6, 0x60, 0x38, 0x0C, 0xC6, 0x7C, 0x00},  // U+0053 (S)
    {0x7E, 0x7E, 0x5A, 0x18, 0x18, 0x18, 0x3C, 0x00},  // U+0054 (T)
    {0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x7C, 0x00},  // U+0055 (U)
    {0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x6C, 0x38, 0x00},  // U+0056 (V)
    {0xC6, 0xC6, 0xC6, 0xD6, 0xFE, 0xEE, 0xC6, 0x00},  // U+0057 (W)
    {0xC6, 0xC6, 0x6C, 0x38, 0x38, 0x6C, 0xC6, 0x00},  // U+0058 (X)
    {0x66, 0x66, 0x66, 0x3C, 0x18, 0x18, 0x3C, 0x00},  // U+0059 (Y)
    {0xFE, 0xC6, 0x8C, 0x18, 0x32, 0x66, 0xFE, 0x00},  // U+005A (Z)
    {0x3C, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3C, 0x00},  // U+005B ([)
    {0xC0, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x02, 0x00},  // U+005C (\)
    {0x3C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x3C, 0x00},  // U+005D (])
    {0x10, 0x38, 0x6C, 0xC6, 0x00, 0x00, 0x00, 0x00},  // U+005E (^)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF},  // U+005F (_)
    {0x30, 0x18, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+0060 (`)
    {0x00, 0x00, 0x1E, 0x30, 0x3E, 0x33, 0x6E, 0x00},  // U+0061 (a)
    {0x07, 0x06, 0x06, 0x3E, 0x66, 0x66, 0x3B, 0x00},  // U+0062 (b)
    {0x00, 0x00, 0x1E, 0x33, 0x03, 0x33, 0x1E, 0x00},  // U+0063 (c)
    {0x38, 0x30, 0x30, 0x3e, 0x33, 0x33, 0x6E, 0x00},  // U+0064 (d)
    {0x00, 0x00, 0x1E, 0x33, 0x3f, 0x03, 0x1E, 0x00},  // U+0065 (e)
    {0x1C, 0x36, 0x06, 0x0f, 0x06, 0x06, 0x0F, 0x00},  // U+0066 (f)
    {0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x1F},  // U+0067 (g)
    {0x07, 0x06, 0x36, 0x6E, 0x66, 0x66, 0x67, 0x00},  // U+0068 (h)
    {0x0C, 0x00, 0x0E, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},  // U+0069 (i)
    {0x30, 0x00, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E},  // U+006A (j)
    {0x07, 0x06, 0x66, 0x36, 0x1E, 0x36, 0x67, 0x00},  // U+006B (k)
    {0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},  // U+006C (l)
    {0x00, 0x00, 0x33, 0x7F, 0x7F, 0x6B, 0x63, 0x00},  // U+006D (m)
    {0x00, 0x00, 0x1F, 0x33, 0x33, 0x33, 0x33, 0x00},  // U+006E (n)
    {0x00, 0x00, 0x1E, 0x33, 0x33, 0x33, 0x1E, 0x00},  // U+006F (o)
    {0x00, 0x00, 0x3B, 0x66, 0x66, 0x3E, 0x06, 0x0F},  // U+0070 (p)
    {0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x78},  // U+0071 (q)
    {0x00, 0x00, 0x3B, 0x6E, 0x66, 0x06, 0x0F, 0x00},  // U+0072 (r)
    {0x00, 0x00, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x00},  // U+0073 (s)
    {0x08, 0x0C, 0x3E, 0x0C, 0x0C, 0x2C, 0x18, 0x00},  // U+0074 (t)
    {0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x6E, 0x00},  // U+0075 (u)
    {0x00, 0x00, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00},  // U+0076 (v)
    {0x00, 0x00, 0x63, 0x6B, 0x7F, 0x7F, 0x36, 0x00},  // U+0077 (w)
    {0x00, 0x00, 0x63, 0x36, 0x1C, 0x36, 0x63, 0x00},  // U+0078 (x)
    {0x00, 0x00, 0x33, 0x33, 0x33, 0x3E, 0x30, 0x1F},  // U+0079 (y)
    {0x00, 0x00, 0x3F, 0x19, 0x0C, 0x26, 0x3F, 0x00},  // U+007A (z)
    {0x38, 0x0C, 0x0C, 0x07, 0x0C, 0x0C, 0x38, 0x00},  // U+007B ({)
    {0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00},  // U+007C (|)
    {0x07, 0x0C, 0x0C, 0x38, 0x0C, 0x0C, 0x07, 0x00},  // U+007D (})
    {0x6E, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+007E (~)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}   // U+007F
};

GraphicViewPortClass* Target_Page() {
  return LogicPage ? LogicPage : &HidPage;
}

const void* Font_For_Type(TextPrintType type) {
  switch (type) {
    case TPF_6POINT:
      return Font6Ptr ? Font6Ptr : FontPtr;
    case TPF_8POINT:
      return Font8Ptr ? Font8Ptr : FontPtr;
    case TPF_3POINT:
      return Font3Ptr ? Font3Ptr : FontPtr;
    case TPF_LED:
      return FontLEDPtr ? FontLEDPtr : FontPtr;
    case TPF_VCR:
      return VCRFontPtr ? VCRFontPtr : FontPtr;
    case TPF_6PT_GRAD:
      return GradFont6Ptr ? GradFont6Ptr : Font6Ptr;
    case TPF_MAP:
      return MapFontPtr ? MapFontPtr : Font6Ptr;
    case TPF_GREEN12:
      return Green12FontPtr ? Green12FontPtr : FontPtr;
    case TPF_GREEN12_GRAD:
      return Green12GradFontPtr ? Green12GradFontPtr : FontPtr;
    case TPF_LASTPOINT:
    default:
      return FontPtr;
  }
}

bool Parse_Font(const void* font_ptr, ParsedFont* out) {
  if (!font_ptr || !out) return false;

#pragma pack(push, 1)
  struct Header {
    std::uint16_t length;
    std::uint8_t compress;
    std::uint8_t data_blocks;
    std::uint16_t info_offset;
    std::uint16_t offset_offset;
    std::uint16_t width_offset;
    std::uint16_t data_offset;
    std::uint16_t height_offset;
  };
#pragma pack(pop)

  const auto* base = static_cast<const unsigned char*>(font_ptr);
  const auto* header = reinterpret_cast<const Header*>(base);
  // The legacy font headers are byte-packed; assert the expected layout so we don't mis-parse
  // offsets and end up with empty glyphs.
  static_assert(sizeof(Header) == 14, "Font header must remain packed");

  // Reject obviously bogus headers (e.g., when falling back to the built-in 8x8 table).
  constexpr std::size_t kMaxFontBytes = 1 * 1024 * 1024;
  const std::uint16_t offsets[] = {header->info_offset,   header->offset_offset, header->width_offset,
                                   header->data_offset,   header->height_offset, header->length};
  auto monotonic = [](const std::uint16_t* arr, std::size_t n) {
    for (std::size_t i = 1; i < n; ++i) {
      if (arr[i] <= arr[i - 1]) return false;
    }
    return true;
  };
  if (!monotonic(offsets, 5)) {
    return false;
  }
  for (std::size_t i = 0; i < 5; ++i) {
    if (offsets[i] == 0 || offsets[i] >= kMaxFontBytes) {
      return false;
    }
  }

  const unsigned char* info_block = base + header->info_offset;
  const int max_height = info_block[4];
  const int max_width = info_block[5];
  if (max_height <= 0 || max_height > 64 || max_width <= 0 || max_width > 64) {
    return false;
  }

  ParsedFont parsed{};
  parsed.base = base;
  parsed.data_base = base + header->data_offset;
  parsed.offsets = reinterpret_cast<const std::uint16_t*>(base + header->offset_offset);
  parsed.widths = base + header->width_offset;
  parsed.heights = base + header->height_offset;
  parsed.max_height = max_height;
  parsed.max_width = max_width;

  *out = parsed;
  return parsed.offsets && parsed.widths && parsed.heights && parsed.base;
}

void Select_Font(TextPrintType flag) {
  static const void* last_font = nullptr;
  const TextPrintType type = static_cast<TextPrintType>(flag & static_cast<TextPrintType>(0x000F));

  const void* requested =
      (type == TPF_LASTPOINT) ? last_font : Font_For_Type(type);
  if (!requested) {
    requested = last_font ? last_font : static_cast<const void*>(kFont8x8);
  }
  last_font = requested;

  g_current_font_valid = Parse_Font(requested, &g_current_font);
  if (!g_current_font_valid) {
    FontHeight = kFontHeight;
  } else {
    FontHeight = std::max(1, g_current_font.max_height);
  }
  FontYSpacing = 1;
  Platform_Set_Fonts(requested, (type == TPF_6PT_GRAD) ? GradFont6Ptr : nullptr, FontHeight,
                     FontYSpacing);
}

void Draw_Glyph(char ch, int x, int y, unsigned fore, unsigned back, bool fill_background) {
  GraphicViewPortClass* page = Target_Page();
  if (!page) return;

  if (!g_current_font_valid) {
    const unsigned char* glyph = kFont8x8[static_cast<unsigned char>(ch) & 0x7F];
    const int rows = std::min(FontHeight, kFontHeight);
    for (int row = 0; row < rows; ++row) {
      for (int col = 0; col < kFontWidth; ++col) {
        const bool bit_set = (glyph[row] & (0x80 >> col)) != 0;
        if (bit_set) {
          page->Put_Pixel(x + col, y + row, static_cast<int>(fore));
        } else if (fill_background) {
          page->Put_Pixel(x + col, y + row, static_cast<int>(back));
        }
      }
    }
    return;
  }

  const unsigned idx = static_cast<unsigned char>(ch);
  const int width = std::max(1, static_cast<int>(g_current_font.widths[idx]));
  const int top_blank = g_current_font.heights[idx * 2];
  const int data_height = g_current_font.heights[idx * 2 + 1];
  const int max_height = g_current_font.max_height;
  const unsigned char* glyph = g_current_font.data_base + g_current_font.offsets[idx];
  const int bytes_per_row = (width + 1) / 2;

  int dest_y = y;
  if (fill_background && top_blank > 0) {
    for (int row = 0; row < top_blank; ++row) {
      for (int col = 0; col < width; ++col) {
        page->Put_Pixel(x + col, dest_y + row, static_cast<int>(back));
      }
    }
  }

  for (int row = 0; row < data_height; ++row) {
    const unsigned char* row_ptr = glyph + row * bytes_per_row;
    for (int col = 0; col < width; ++col) {
      const unsigned char byte = row_ptr[col / 2];
      const unsigned char nibble = (col & 1) ? (byte >> 4) & 0x0F : byte & 0x0F;
      if (nibble) {
        page->Put_Pixel(x + col, dest_y + top_blank + row, static_cast<int>(fore));
      } else if (fill_background) {
        page->Put_Pixel(x + col, dest_y + top_blank + row, static_cast<int>(back));
      }
    }
  }

  const int bottom_blank = std::max(0, max_height - top_blank - data_height);
  if (fill_background && bottom_blank > 0) {
    const int start_y = dest_y + top_blank + data_height;
    for (int row = 0; row < bottom_blank; ++row) {
      for (int col = 0; col < width; ++col) {
        page->Put_Pixel(x + col, start_y + row, static_cast<int>(back));
      }
    }
  }
}

void Draw_String(const char* text, unsigned x, unsigned y, unsigned fore, unsigned back,
                 TextPrintType flag) {
  if (!text) return;

  Select_Font(flag);
  const int width = String_Pixel_Width(text);
  switch (flag & (TPF_CENTER | TPF_RIGHT)) {
    case TPF_CENTER:
      x -= static_cast<unsigned>(width / 2);
      break;
    case TPF_RIGHT:
      x -= static_cast<unsigned>(width);
      break;
    default:
      break;
  }

  const bool shadow = (flag & TPF_NOSHADOW) == 0;
  const bool fill_background = back != TBLACK;
  unsigned cursor_x = x;
  for (const char* ptr = text; *ptr; ++ptr) {
    const unsigned char ch = static_cast<unsigned char>(*ptr);
    const int glyph_width = g_current_font_valid
                                ? std::max(1, static_cast<int>(g_current_font.widths[ch]))
                                : kFontWidth;
    if (*ptr == '\t') {
      const int tab_width =
          g_current_font_valid ? std::max(g_current_font.max_width, kFontWidth) : kFontWidth;
      cursor_x += static_cast<unsigned>((tab_width + FontXSpacing) * 4);
      continue;
    }
    if (shadow) {
      Draw_Glyph(*ptr, static_cast<int>(cursor_x + 1), static_cast<int>(y + 1), TBLACK, back,
                 fill_background);
    }
    Draw_Glyph(*ptr, static_cast<int>(cursor_x), static_cast<int>(y), fore, back, fill_background);
    cursor_x += static_cast<unsigned>(glyph_width + FontXSpacing);
  }
}

const char* Lookup_Text(int id) {
  switch (id) {
    case TXT_NONE:
      return "";
    case TXT_START_NEW_GAME:
      return "Start New Game";
    case TXT_LOAD_MISSION:
      return "Load Mission";
    case TXT_MULTIPLAYER_GAME:
      return "Multiplayer";
    case TXT_INTRO:
      return "Intro";
    case TXT_EXIT_GAME:
      return "Exit Game";
    case TXT_INTERNET:
      return "Internet Game";
    case TXT_NEW_MISSIONS:
      return "New Missions";
    case TXT_BONUS_MISSIONS:
      return "Bonus Missions";
    default:
      break;
  }

  static std::unordered_map<int, std::string> cache;
  auto it = cache.find(id);
  if (it != cache.end()) {
    return it->second.c_str();
  }
  cache[id] = "TXT_" + std::to_string(id);
  return cache[id].c_str();
}

}  // namespace

int Char_Pixel_Width(int ch) {
  const bool is_tab = (ch == '\t');
  if (g_current_font_valid) {
    if (is_tab) {
      return (std::max(g_current_font.max_width, kFontWidth) + FontXSpacing) * 4;
    }
    return std::max(1, static_cast<int>(g_current_font.widths[static_cast<unsigned char>(ch)])) +
           FontXSpacing;
  }
  if (is_tab) {
    return (kFontWidth + FontXSpacing) * 4;
  }
  return kFontWidth + FontXSpacing;
}

int String_Pixel_Width(char const* text) {
  if (!text) return 0;
  int width = 0;
  for (const char* ptr = text; *ptr; ++ptr) {
    const unsigned char ch = static_cast<unsigned char>(*ptr);
    const int char_width =
        g_current_font_valid ? std::max(1, static_cast<int>(g_current_font.widths[ch]))
                             : kFontWidth;
    if (*ptr == '\t') {
      const int tab_width =
          g_current_font_valid ? std::max(g_current_font.max_width, kFontWidth) : kFontWidth;
      width += (tab_width + FontXSpacing) * 4;
    } else {
      width += char_width + FontXSpacing;
    }
  }
  return width;
}

void Conquer_Init_Fonts() { Select_Font(TPF_8POINT); }

void Conquer_Clip_Text_Print(char const* text, unsigned x, unsigned y, unsigned fore,
                             unsigned back, TextPrintType flag, unsigned width, int const*) {
  if (!text) return;
  std::string clipped;
  unsigned current_width = 0;
  for (const char* ptr = text; *ptr; ++ptr) {
    const unsigned char ch = static_cast<unsigned char>(*ptr);
    unsigned advance = (ch == '\t') ? static_cast<unsigned>(kFontWidth * 4)
                                    : static_cast<unsigned>(kFontWidth);
    if (current_width + advance > width) {
      break;
    }
    clipped.push_back(*ptr);
    current_width += advance;
  }
  Draw_String(clipped.c_str(), x, y, fore, back, flag);
}

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wvarargs"
#endif

void Fancy_Text_Print(char const* text, unsigned x, unsigned y, unsigned fore, unsigned back,
                      TextPrintType flag, ...) {
  if (!text) {
    Select_Font(flag);
    return;
  }

  char buffer[512]{};
  va_list args;
  va_start(args, flag);
  std::vsnprintf(buffer, sizeof(buffer), text, args);
  va_end(args);

  Draw_String(buffer, x, y, fore, back, flag);
}

void Fancy_Text_Print(int text, unsigned x, unsigned y, unsigned fore, unsigned back,
                      TextPrintType flag, ...) {
  if (text == TXT_NONE) {
    Select_Font(flag);
    return;
  }

  char buffer[512]{};
  va_list args;
  va_start(args, flag);
  std::vsnprintf(buffer, sizeof(buffer), Lookup_Text(text), args);
  va_end(args);

  Draw_String(buffer, x, y, fore, back, flag);
}

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

void CC_Draw_Text(int text) { Fancy_Text_Print(text, 0, 0, WHITE, TBLACK, TPF_8POINT); }

char const* Extract_String(char const* /*text*/, int index) { return Lookup_Text(index); }
