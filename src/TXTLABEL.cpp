#include "function.h"
#include "txtlabel.h"

TextLabelClass::TextLabelClass(char* txt, int x, int y, int color, TextPrintType style)
    : GadgetClass(x, y, 1, 1, 0, 0) {
  Text = txt;
  Color = color;
  Style = style;
  UserData = 0;
  PixWidth = -1;
  Segments = 0;
  CRC = 0;
}

int TextLabelClass::Draw_Me(int forced) {
  if (GadgetClass::Draw_Me(forced)) {
    if (PixWidth == -1) {
      Fancy_Text_Print("%s", X, Y, Color, TBLACK, Style, Text);
    } else {
      Conquer_Clip_Text_Print(Text, X, Y, Color, TBLACK, Style, PixWidth);
    }
    return true;
  }
  return false;
}

