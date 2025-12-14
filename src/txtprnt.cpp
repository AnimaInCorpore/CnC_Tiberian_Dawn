// Portable replacements for legacy TXTPRNT.ASM helpers.
#include "legacy/wwlib32.h"
#include "legacy/function.h"
#include "legacy/externs.h"
#include <cstring>
#include <string>

// Forward declarations for the text helpers we call into.
extern int Char_Pixel_Width(int ch);
extern int FontHeight;
extern int FontYSpacing;

extern "C" {

// Mirror the 256-byte ColorXlat table used by the original asm.
static unsigned char s_font_color_xlat[256];

static void Init_Font_Palette() {
  static bool init = false;
  if (init) return;
  init = true;
  // Table layout: first row contains 0x00..0x0F; subsequent rows have the
  // row index in the first column and zeros otherwise. This matches the
  // original TXTPRNT.ASM data block so callers that copy/restore the table
  // behave the same.
  for (int r = 0; r < 16; ++r) {
    for (int c = 0; c < 16; ++c) {
      const int idx = r * 16 + c;
      if (r == 0) s_font_color_xlat[idx] = static_cast<unsigned char>(c & 0xFF);
      else if (c == 0) s_font_color_xlat[idx] = static_cast<unsigned char>(r & 0xFF);
      else s_font_color_xlat[idx] = 0;
    }
  }
}

// Returns a pointer to the 256-byte font palette translation table.
void const* Get_Font_Palette_Ptr(void) {
  Init_Font_Palette();
  return s_font_color_xlat;
}

// High-level replacement for Buffer_Print that routes through the modern
// text pipeline but preserves the legacy line-feed behavior expected by
// callers. Returns the x pixel for the next draw position or 0 on overflow.
int Buffer_Print(void* this_object, const char* string, int x_pixel, int y_pixel,
                 unsigned fcolor, unsigned bcolor) {
  if (!this_object || !string) return 0;

  GraphicViewPortClass* page = static_cast<GraphicViewPortClass*>(this_object);
  GraphicViewPortClass* oldpage = Set_Logic_Page(*page);

  const int original_x = x_pixel;
  const int vpwidth = page->Get_Width();
  const int vpheight = page->Get_Height();

  // Early height check: if even the max font row doesn't fit, signal overflow.
  if (y_pixel + FontHeight > vpheight) {
    Set_Logic_Page(*oldpage);
    return 0;
  }

  // Walk the string manually so we can emulate the ASM line-feed handling.
  for (const char* p = string; *p != '\0'; ) {
    unsigned char ch = static_cast<unsigned char>(*p);

    if (ch == 10 || ch == 13) {
      // Line feed behaviour: LF (10) -> left edge (0), CR (13) -> original margin
      unsigned char kind = ch;
      ++p;  // consume the line feed char
      y_pixel += FontHeight + FontYSpacing;
      if (y_pixel + FontHeight > vpheight) {
        Set_Logic_Page(*oldpage);
        return 0; // overflow
      }
      x_pixel = (kind == 10) ? 0 : original_x;
      continue;
    }

    // Determine the pixel width of this character and wrap if it won't fit.
    const int char_w = Char_Pixel_Width(ch);
    if (x_pixel + char_w > vpwidth) {
      // Force a line feed and re-process the same character.
      y_pixel += FontHeight + FontYSpacing;
      if (y_pixel + FontHeight > vpheight) {
        Set_Logic_Page(*oldpage);
        return 0; // overflow
      }
      x_pixel = 0;
      continue; // re-check same character at new line
    }

    // Draw the single character using the existing Fancy_Text_Print builder.
    char buf[2] = {static_cast<char>(ch), '\0'};
    Fancy_Text_Print(buf, static_cast<unsigned>(x_pixel), static_cast<unsigned>(y_pixel),
                     fcolor, bcolor, TPF_LASTPOINT);

    x_pixel += char_w;
    ++p; // advance to next character
  }

  Set_Logic_Page(*oldpage);
  return x_pixel;
}

} // extern "C"
