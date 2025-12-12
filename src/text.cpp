/*
**	Simple text rendering helpers for the SDL port.
*/

#include "legacy/function.h"
#include "legacy/externs.h"
#include "legacy/wwlib32.h"

#include <cstdarg>
#include <cstdio>
#include <cstddef>
#include <algorithm>
#include <cstdint>
#include <limits>
#include <string>
#include <unordered_map>

int FontHeight = 8;
int FontYSpacing = 1;
int FontXSpacing = 0;

namespace {

constexpr int kFontWidth = 8;
constexpr int kFontHeight = 8;

struct ParsedFont {
  const unsigned char* base = nullptr;
  const std::uint16_t* offsets = nullptr;
  const unsigned char* widths = nullptr;
  const unsigned char* heights = nullptr;  // two bytes per character: top blank, data height
  std::size_t length = 0;
  std::size_t glyph_count = 0;
  int max_height = kFontHeight;
  int max_width = kFontWidth;
};

ParsedFont g_current_font{};
bool g_current_font_valid = false;
bool g_reported_font_failure = false;

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

void Report_Font_Failure(const char* message) {
  if (g_reported_font_failure) return;
  g_reported_font_failure = true;
  CCDebugString(message ? message : "Font asset missing or invalid.\n");
}

void Clear_Font_Failure_Report() { g_reported_font_failure = false; }

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

  // Reject obviously bogus headers so we don't treat random memory as a valid font asset.
  constexpr std::size_t kMaxFontBytes = std::numeric_limits<std::uint16_t>::max();
  if (header->length == 0 || header->length > kMaxFontBytes) {
    return false;
  }
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

  const std::size_t offset_bytes =
      static_cast<std::size_t>(header->width_offset) - static_cast<std::size_t>(header->offset_offset);
  if (offset_bytes == 0 || (offset_bytes % sizeof(std::uint16_t)) != 0) {
    return false;
  }
  const std::size_t glyph_count = offset_bytes / sizeof(std::uint16_t);
  const std::size_t width_bytes =
      static_cast<std::size_t>(header->data_offset) - static_cast<std::size_t>(header->width_offset);
  if (width_bytes != glyph_count) {
    return false;
  }
  if (header->height_offset < header->data_offset) {
    return false;
  }
  const std::size_t height_bytes =
      static_cast<std::size_t>(header->length) - static_cast<std::size_t>(header->height_offset);
  if (height_bytes / 2 < glyph_count) {
    return false;
  }

  const unsigned char* info_block = base + header->info_offset;
  const int max_height = info_block[4];
  const int max_width = info_block[5];
  if (max_height <= 0 || max_height > 64 || max_width <= 0 || max_width > 64) {
    return false;
  }

  const std::uint16_t* offset_block =
      reinterpret_cast<const std::uint16_t*>(base + header->offset_offset);
  for (std::size_t i = 0; i < glyph_count; ++i) {
    if (offset_block[i] >= header->length) {
      return false;
    }
  }

  ParsedFont parsed{};
  parsed.base = base;
  parsed.offsets = offset_block;
  parsed.widths = base + header->width_offset;
  parsed.heights = base + header->height_offset;
  parsed.length = header->length;
  parsed.glyph_count = glyph_count;
  parsed.max_height = max_height;
  parsed.max_width = max_width;

  *out = parsed;
  return parsed.offsets && parsed.widths && parsed.heights && parsed.base;
}

void Select_Font(TextPrintType flag) {
  static const void* last_font = nullptr;
  const TextPrintType type = static_cast<TextPrintType>(flag & static_cast<TextPrintType>(0x000F));

  const void* requested = (type == TPF_LASTPOINT) ? last_font : Font_For_Type(type);
  if (!requested) {
    g_current_font_valid = false;
    FontHeight = 0;
    FontYSpacing = 0;
    Platform_Set_Fonts(nullptr, nullptr, FontHeight, FontYSpacing);
    Report_Font_Failure("Missing font asset from CD data; text rendering disabled.\n");
    return;
  }

  g_current_font_valid = Parse_Font(requested, &g_current_font);
  if (!g_current_font_valid) {
    FontHeight = 0;
    FontYSpacing = 0;
    Platform_Set_Fonts(nullptr, nullptr, FontHeight, FontYSpacing);
    Report_Font_Failure("Invalid font asset; ensure CD font archives are present.\n");
    return;
  }

  Clear_Font_Failure_Report();
  last_font = requested;
  FontHeight = std::max(1, g_current_font.max_height);
  FontYSpacing = 1;
  Platform_Set_Fonts(requested, (type == TPF_6PT_GRAD) ? GradFont6Ptr : nullptr, FontHeight,
                     FontYSpacing);
}

void Draw_Glyph(char ch, int x, int y, unsigned fore, unsigned back, bool fill_background) {
  GraphicViewPortClass* page = Target_Page();
  if (!page || !g_current_font_valid) return;

  const auto translate = [&](unsigned char nibble) -> int {
    // Match the legacy ColorXlat table: 0 = background (or transparent), 1 = foreground,
    // everything else uses the nibble directly so gradient palettes stay intact.
    if (nibble == 0) {
      return fill_background ? static_cast<int>(back) : -1;
    }
    if (nibble == 1) {
      return static_cast<int>(fore);
    }
    return static_cast<int>(nibble);
  };

  const unsigned idx = static_cast<unsigned char>(ch);
  if (idx >= g_current_font.glyph_count) return;
  const int width = std::max(1, static_cast<int>(g_current_font.widths[idx]));
  const int top_blank = g_current_font.heights[idx * 2];
  const int data_height = g_current_font.heights[idx * 2 + 1];
  const int max_height = g_current_font.max_height;
  const std::uint16_t glyph_offset = g_current_font.offsets[idx];
  if (glyph_offset >= g_current_font.length) return;
  const unsigned char* glyph = g_current_font.base + glyph_offset;
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
      const int color = translate(nibble);
      if (color >= 0) {
        // Skip zero to preserve transparency when the caller requested no background fill.
        if (color != 0) {
          page->Put_Pixel(x + col, dest_y + top_blank + row, color);
        }
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
  if (!g_current_font_valid) return;

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
    if (ch >= g_current_font.glyph_count) {
      continue;
    }
    const int glyph_width = std::max(1, static_cast<int>(g_current_font.widths[ch]));
    if (*ptr == '\t') {
      const int tab_width = std::max(g_current_font.max_width, kFontWidth);
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
  if (!g_current_font_valid) return 0;
  const bool is_tab = (ch == '\t');
  if (is_tab) {
    return (std::max(g_current_font.max_width, kFontWidth) + FontXSpacing) * 4;
  }
  if (static_cast<unsigned>(static_cast<unsigned char>(ch)) >= g_current_font.glyph_count) {
    return 0;
  }
  return std::max(1, static_cast<int>(g_current_font.widths[static_cast<unsigned char>(ch)])) +
         FontXSpacing;
}

int String_Pixel_Width(char const* text) {
  if (!text || !g_current_font_valid) return 0;
  int width = 0;
  for (const char* ptr = text; *ptr; ++ptr) {
    const unsigned char ch = static_cast<unsigned char>(*ptr);
    if (ch >= g_current_font.glyph_count) {
      continue;
    }
    const int char_width = std::max(1, static_cast<int>(g_current_font.widths[ch]));
    if (*ptr == '\t') {
      const int tab_width = std::max(g_current_font.max_width, kFontWidth);
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
  Select_Font(flag);
  if (!g_current_font_valid) return;

  std::string clipped;
  unsigned current_width = 0;
  for (const char* ptr = text; *ptr; ++ptr) {
    const unsigned advance = static_cast<unsigned>(Char_Pixel_Width(*ptr));
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
