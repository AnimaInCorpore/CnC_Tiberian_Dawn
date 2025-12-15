#include "legacy/compat.h"

void Set_Window(int window, int x, int y, int w, int h)
{
    WindowList[window][WINDOWWIDTH] = w >> 3;
    WindowList[window][WINDOWHEIGHT] = h;
    WindowList[window][WINDOWX] = x >> 3;
    WindowList[window][WINDOWY] = y;
}
