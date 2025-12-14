/*
**	Dialog helpers (partial port)
*/

#include "legacy/function.h"
#include <cstring>

// Draw a dialog background box.
void Dialog_Box(int x, int y, int w, int h) {
  Draw_Box(x, y, w, h, BOXSTYLE_GREEN_BORDER, true);
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
