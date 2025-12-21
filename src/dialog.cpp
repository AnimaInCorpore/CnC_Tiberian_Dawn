/*
**	Dialog helpers (partial port)
**
**	Note: `Draw_Box` is the canonical Win95 UI box renderer. It is used by most
**	gadgets and dialog helpers and must match the original behavior closely.
*/

#include "legacy/compat.h"
#include "legacy/externs.h"
#include "legacy/function.h"
#include "legacy/mixfile.h"

#include <cstring>

// Draw a dialog background box.
void Dialog_Box(int x, int y, int w, int h) {
  Draw_Box(x, y, w, h, BOXSTYLE_GREEN_BORDER, true);
}

// Win95 textured UI fill used by the green dialog/menu styles.
static void CC_Texture_Fill(void const* shapefile, int shapenum, int xpos, int ypos, int width, int height) {
  if (!shapefile || shapenum == -1) return;

  unsigned long shape_size = Build_Frame(shapefile, static_cast<unsigned short>(shapenum), ShapeBuffer);
  if (!shape_size) return;

  if (Get_Last_Frame_Length() > _ShapeBufferSize) {
    return;
  }

  auto* shape_pointer = static_cast<unsigned char*>(Get_Shape_Header_Data(reinterpret_cast<void*>(shape_size)));
  const int source_width = Get_Build_Frame_Width(shapefile);
  const int source_height = Get_Build_Frame_Height(shapefile);
  if (!shape_pointer || source_width <= 0 || source_height <= 0) return;

  for (int y = 0; y < height; ++y) {
    const int src_y = y % source_height;
    for (int x = 0; x < width; ++x) {
      const int src_x = x % source_width;
      LogicPage->Put_Pixel(xpos + x, ypos + y, shape_pointer[src_y * source_width + src_x]);
    }
  }
}

void Draw_Box(int x, int y, int w, int h, BoxStyleEnum up, bool filled) {
  static BoxStyleType const ButtonColors[BOXSTYLE_COUNT] = {
      // Filler, Shadow, Hilite, Corner colors
      {LTGREY, WHITE, DKGREY, LTGREY},      // 0 Button is down.
      {LTGREY, DKGREY, WHITE, LTGREY},      // 1 Button is up w/border.
      {LTBLUE, BLUE, LTCYAN, LTBLUE},       // 2 Raised blue.
      {DKGREY, WHITE, BLACK, DKGREY},       // 3 Button is disabled down.
      {DKGREY, BLACK, WHITE, LTGREY},       // 4 Button is disabled up.
      {LTGREY, DKGREY, WHITE, LTGREY},      // 5 Button is up w/arrows.
      {CC_GREEN_BKGD, 14, 12, 13},          // 6 Button is down.
      {CC_GREEN_BKGD, 12, 14, 13},          // 7 Button is up w/border.
      {DKGREY, WHITE, BLACK, DKGREY},       // 8 Button is disabled down.
      {DKGREY, BLACK, LTGREY, DKGREY},      // 9 Button is disabled up.
      {BLACK, 14, 14, BLACK},               // 10 List box.
      {BLACK, 14, 14, BLACK},               // 11 Main dialog box.
  };

  // The original code treats w/h as extents and immediately decrements to get
  // the inclusive bottom-right coordinate.
  --w;
  --h;
  BoxStyleType const& style = ButtonColors[up];

  if (filled) {
    if (style.Filler == CC_GREEN_BKGD) {
      CC_Texture_Fill(MixFileClass::Retrieve("BTEXTURE.SHP"), InMainLoop, x, y, w, h);
    } else {
      LogicPage->Fill_Rect(x, y, x + w, y + h, style.Filler);
    }
  }

  switch (up) {
    case (BOXSTYLE_GREEN_BOX):
      LogicPage->Draw_Rect(x, y, x + w, y + h, style.Highlight);
      break;

    case (BOXSTYLE_GREEN_BORDER):
      LogicPage->Draw_Rect(x + 1, y + 1, x + w - 1, y + h - 1, style.Highlight);
      break;

    default:
      LogicPage->Draw_Line(x, y + h, x + w, y + h, style.Shadow);
      LogicPage->Draw_Line(x + w, y, x + w, y + h, style.Shadow);

      LogicPage->Draw_Line(x, y, x + w, y, style.Highlight);
      LogicPage->Draw_Line(x, y, x, y + h, style.Highlight);

      LogicPage->Put_Pixel(x, y + h, style.Corner);
      LogicPage->Put_Pixel(x + w, y, style.Corner);
      break;
  }
}

// Split a long string to lines not exceeding maxlinelen (in pixels).
int Format_Window_String(char* string, int maxlinelen, int& width, int& height) {
  if (!string) {
    width = 0;
    height = 0;
    return 0;
  }

  int lines = 0;
  width = 0;
  height = 0;

  char* ptr = string;
  while (*ptr) {
    int linelen = 0;
    char* linestart = ptr;
    height += FontHeight + FontYSpacing;
    ++lines;

    while (linelen < maxlinelen && *ptr != '\r' && *ptr != '\0') {
      linelen += Char_Pixel_Width(*ptr);
      ++ptr;
    }

    if (linelen >= maxlinelen) {
      // Back up to last space
      while (ptr > linestart && *ptr != ' ' && *ptr != '\r' && *ptr != '\0') {
        --ptr;
        linelen -= Char_Pixel_Width(*ptr);
      }
    }

    if (linelen > width) width = linelen;

    if (*ptr) {
      // mark end-of-line with CR to work with existing print helpers
      *ptr++ = '\r';
    }
  }

  return lines;
}

// Draw a fancy window box based on the window index table.
void Window_Box(WindowNumberType window, BoxStyleEnum style) {
  int x = WindowList[window][WINDOWX] << 3;
  int y = WindowList[window][WINDOWY];
  int w = WindowList[window][WINDOWWIDTH] << 3;
  int h = WindowList[window][WINDOWHEIGHT];

  if (LogicPage == (&SeenBuff)) Conditional_Hide_Mouse(x, y, x + w, y + h);

  Draw_Box(x, y, w, h, style, true);

  static int _border[BOXSTYLE_COUNT][2] = {
      {0, 0}, {2, 4}, {1, 1}, {2, 1}, {0, 0}, {20, 0}, {0, 0}, {2, 4}, {0, 0}, {20, 0}, {0, 1}};

  int border = 0;
  if (style >= 0 && style < BOXSTYLE_COUNT) border = _border[style][1];

  if (border) {
    Draw_Box(x + border, y + border, w - (border << 1), h - (border << 1), style, false);
  }

  if (LogicPage == (&SeenBuff)) Conditional_Show_Mouse();
}
