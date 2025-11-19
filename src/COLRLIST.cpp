#include "function.h"
#include "colrlist.h"

namespace {
inline TextPrintType Combine_Text_Flags(TextPrintType base, TextPrintType extra) {
  return static_cast<TextPrintType>(static_cast<int>(base) | static_cast<int>(extra));
}

void Draw_Rectangle(GraphicViewPortClass* page, int left, int top, int right, int bottom, int color) {
  page->Draw_Line(left, top, right, top, color);
  page->Draw_Line(left, bottom, right, bottom, color);
  page->Draw_Line(left, top, left, bottom, color);
  page->Draw_Line(right, top, right, bottom, color);
}
}  // namespace

ColorListClass::ColorListClass(int id, int x, int y, int w, int h, TextPrintType flags,
                               void const* up, void const* down)
    : ListClass(id, x, y, w, h, flags, up, down), Style(SELECT_HIGHLIGHT), SelectColor(-1) {}

ColorListClass::~ColorListClass() { Colors.Clear(); }

int ColorListClass::Add_Item(char const* text, char color) {
  if (!text) {
    return List.Count() - 1;
  }

  const int index = ListClass::Add_Item(text);
  Colors.Add(color);
  return index;
}

int ColorListClass::Add_Item(int text, char color) {
  const int index = ListClass::Add_Item(text);
  if (text != TXT_NONE) {
    Colors.Add(color);
  }
  return index;
}

void ColorListClass::Remove_Item(char const* text) {
  if (!text) {
    return;
  }

  const int index = List.ID(text);
  if (index != -1) {
    Colors.Delete(index);
    ListClass::Remove_Item(text);
  }
}

void ColorListClass::Set_Selected_Style(SelectStyleType style, int color) {
  Style = style;
  SelectColor = color;
}

void ColorListClass::Draw_Entry(int index, int x, int y, int width, int selected) {
  const bool has_color = index >= 0 && index < Colors.Count();
  const int entry_color = has_color ? Colors[index] : WHITE;
  char const* text = List[index];
  if (!text) {
    return;
  }

  if (!selected) {
    Conquer_Clip_Text_Print(text, x, y, entry_color, TBLACK, TextFlags, width, Tabs);
    return;
  }

  const bool use_grad = (TextFlags & TPF_6PT_GRAD) != 0;
  const int highlight_color = (SelectColor == -1) ? entry_color : SelectColor;
  TextPrintType highlight_flags = TextFlags;

  switch (Style) {
    case SELECT_NONE:
      Conquer_Clip_Text_Print(text, x, y, entry_color, TBLACK, TextFlags, width, Tabs);
      break;

    case SELECT_HIGHLIGHT:
      if (use_grad) {
        highlight_flags = Combine_Text_Flags(highlight_flags, TPF_BRIGHT_COLOR);
      }
      Conquer_Clip_Text_Print(text, x, y, highlight_color, TBLACK, highlight_flags, width, Tabs);
      break;

    case SELECT_BOX:
      Draw_Rectangle(LogicPage, x, y, x + width - 2, y + LineHeight - 2, highlight_color);
      Conquer_Clip_Text_Print(text, x, y, entry_color, TBLACK, TextFlags, width, Tabs);
      break;

    case SELECT_BAR: {
      const int right = x + width - (use_grad ? 1 : 2);
      const int bottom = y + LineHeight - (use_grad ? 1 : 2);
      LogicPage->Fill_Rect(x, y, right, bottom, highlight_color);
      TextPrintType bar_flags = TextFlags;
      if (use_grad) {
        bar_flags = Combine_Text_Flags(bar_flags, TPF_BRIGHT_COLOR);
      }
      Conquer_Clip_Text_Print(text, x, y, entry_color, TBLACK, bar_flags, width, Tabs);
      break;
    }

    case SELECT_INVERT:
      if (use_grad) {
        LogicPage->Fill_Rect(x, y, x + width - 1, y + LineHeight - 1, entry_color);
        Conquer_Clip_Text_Print(text, x, y, BLACK, TBLACK, TextFlags, width, Tabs);
      } else {
        LogicPage->Fill_Rect(x, y, x + width - 2, y + LineHeight - 2, entry_color);
        Conquer_Clip_Text_Print(text, x, y, LTGREY, TBLACK, TextFlags, width, Tabs);
      }
      break;
  }
}
