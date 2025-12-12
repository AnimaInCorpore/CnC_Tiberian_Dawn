#include "function.h"
#include "textbtn.h"

TextButtonClass::TextButtonClass()
    : ToggleClass(0, 0, 0, 0, 0), IsBlackBorder(0), String(nullptr), PrintFlags(TPF_8POINT) {}

TextButtonClass::TextButtonClass(unsigned id, char const* text, TextPrintType style, int x, int y, int w,
                                 int h, int blackborder)
    : ToggleClass(id, x, y, w, h), IsBlackBorder(blackborder), String(nullptr), PrintFlags(style) {
  Set_Text(text);
  if (w == -1 || h == -1) {
    Fancy_Text_Print(TXT_NONE, 0, 0, TBLACK, TBLACK, PrintFlags);
    if (w == -1 && String) {
      Width = String_Pixel_Width(String) + 8;
    }
    if (h == -1) {
      Height = FontHeight + FontYSpacing + 2;
    }
  }
}

TextButtonClass::TextButtonClass(unsigned id, int text, TextPrintType style, int x, int y, int w, int h,
                                 int blackborder)
    : ToggleClass(id, x, y, w, h), IsBlackBorder(blackborder), String(nullptr), PrintFlags(style) {
  Set_Text(text);
  if (w == -1 || h == -1) {
    Fancy_Text_Print(TXT_NONE, 0, 0, TBLACK, TBLACK, PrintFlags);
    if (w == -1 && String) {
      Width = String_Pixel_Width(String) + 8;
    }
    if (h == -1) {
      Height = FontHeight + FontYSpacing + 2;
    }
  }
}

int TextButtonClass::Draw_Me(int forced) {
  if (!ControlClass::Draw_Me(forced)) {
    return false;
  }

  if (LogicPage == &SeenBuff) {
    Conditional_Hide_Mouse(X, Y, X + Width, Y + Height);
  }

  Draw_Background();
  Draw_Text(String);

  if (LogicPage == &SeenBuff) {
    Conditional_Show_Mouse();
  }
  return true;
}

void TextButtonClass::Set_Text(char const* text, bool resize) {
  String = text;
  Flag_To_Redraw();
  if (resize && String) {
    Fancy_Text_Print(TXT_NONE, 0, 0, TBLACK, TBLACK, PrintFlags);
    Width = String_Pixel_Width(String) + 8;
    Height = FontHeight + FontYSpacing + 2;
  }
}

void TextButtonClass::Set_Text(int text, bool resize) {
  if (text != TXT_NONE) {
    Set_Text(Text_String(text), resize);
  }
}

void TextButtonClass::Draw_Background() {
  if (IsBlackBorder) {
    LogicPage->Draw_Rect(X - 1, Y - 1, X + Width + 2, Y + Height + 2, BLACK);
  }

  BoxStyleEnum style;
  if (PrintFlags & TPF_6PT_GRAD) {
    if (IsDisabled) {
      style = BOXSTYLE_GREEN_DIS_RAISED;
    } else if (IsPressed) {
      style = BOXSTYLE_GREEN_DOWN;
    } else {
      style = BOXSTYLE_GREEN_RAISED;
    }
  } else {
    if (IsDisabled) {
      style = BOXSTYLE_DIS_RAISED;
    } else if (IsPressed) {
      style = BOXSTYLE_DOWN;
    } else {
      style = BOXSTYLE_RAISED;
    }
  }
  Draw_Box(X, Y, Width, Height, style, true);
}

void TextButtonClass::Draw_Text(char const* text) {
  if (!text) {
    return;
  }

  const int anchor_x = X + (Width >> 1);
  const int anchor_y = Y + std::max(0, (Height - FontHeight) / 2);

  if (PrintFlags & TPF_6PT_GRAD) {
    TextPrintType flags = static_cast<TextPrintType>(0);
    int color = WHITE;

    if (!IsDisabled) {
      if (IsPressed || IsOn) {
        flags = static_cast<TextPrintType>(TPF_USE_GRAD_PAL | TPF_BRIGHT_COLOR);
      } else {
        flags = static_cast<TextPrintType>(TPF_USE_GRAD_PAL | TPF_MEDIUM_COLOR);
      }
    } else {
      color = DKGREY;
    }
    Fancy_Text_Print(text, anchor_x, anchor_y, color, TBLACK,
                     static_cast<TextPrintType>(PrintFlags | flags | TPF_CENTER));
    return;
  }

  int color;
  if (IsDisabled) {
    color = LTGREY;
  } else if (IsPressed) {
    color = (PrintFlags & TPF_NOSHADOW) ? DKGREY : LTGREY;
  } else {
    color = WHITE;
  }

  Fancy_Text_Print(text, anchor_x, anchor_y, IsOn ? RED : color, TBLACK,
                   static_cast<TextPrintType>(PrintFlags | TPF_CENTER));
}
