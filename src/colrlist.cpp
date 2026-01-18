#include "colrlist.h"

#include <cstring>

ColorListClass::ColorListClass(int id, int x, int y, int w, int h, TextPrintType flags, void const* up, void const* down)
    : ListClass(id, x, y, w, h, flags, up, down), Style(SELECT_HIGHLIGHT), SelectColor(-1) {}

ColorListClass::~ColorListClass(void) { Colors.Clear(); }

int ColorListClass::Add_Item(char const* text, char color) {
    Colors.Add(color);
    return ListClass::Add_Item(text);
}

int ColorListClass::Add_Item(int text, char color) {
    Colors.Add(color);
    return ListClass::Add_Item(text);
}

void ColorListClass::Remove_Item(char const* text) {
    int index = -1;
    if (text) {
        for (int i = 0; i < List.Count(); ++i) {
            if (!List[i]) continue;
            if (std::strcmp(List[i], text) == 0) {
                index = i;
                break;
            }
        }
    }

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
    if (!selected) {
        Conquer_Clip_Text_Print(List[index], x, y, Colors[index], TBLACK, TextFlags, width, Tabs);
        return;
    }

    int color = (SelectColor == -1) ? Colors[index] : SelectColor;

    switch (Style) {
        case SELECT_NONE:
            Conquer_Clip_Text_Print(List[index], x, y, Colors[index], TBLACK, TextFlags, width, Tabs);
            break;
        case SELECT_HIGHLIGHT:
            if (TextFlags & TPF_6PT_GRAD) {
                Conquer_Clip_Text_Print(List[index], x, y, color, TBLACK, TextFlags | TPF_BRIGHT_COLOR, width, Tabs);
            } else {
                Conquer_Clip_Text_Print(List[index], x, y, color, TBLACK, TextFlags, width, Tabs);
            }
            break;
        case SELECT_BOX:
            if (LogicPage) LogicPage->Draw_Rect(x, y, x + width - 2, y + LineHeight - 2, color);
            Conquer_Clip_Text_Print(List[index], x, y, Colors[index], TBLACK, TextFlags, width, Tabs);
            break;
        case SELECT_BAR:
            if (LogicPage) {
                if (TextFlags & TPF_6PT_GRAD) {
                    LogicPage->Fill_Rect(x, y, x + width - 1, y + LineHeight - 1, SelectColor);
                } else {
                    LogicPage->Fill_Rect(x, y, x + width - 2, y + LineHeight - 2, SelectColor);
                }
            }
            if (TextFlags & TPF_6PT_GRAD) {
                Conquer_Clip_Text_Print(List[index], x, y, Colors[index], TBLACK, TextFlags | TPF_BRIGHT_COLOR, width,
                                       Tabs);
            } else {
                Conquer_Clip_Text_Print(List[index], x, y, Colors[index], TBLACK, TextFlags, width, Tabs);
            }
            break;
        case SELECT_INVERT:
            if (LogicPage) {
                if (TextFlags & TPF_6PT_GRAD) {
                    LogicPage->Fill_Rect(x, y, x + width - 1, y + LineHeight - 1, Colors[index]);
                } else {
                    LogicPage->Fill_Rect(x, y, x + width - 2, y + LineHeight - 2, Colors[index]);
                }
            }
            if (TextFlags & TPF_6PT_GRAD) {
                Conquer_Clip_Text_Print(List[index], x, y, BLACK, TBLACK, TextFlags, width, Tabs);
            } else {
                Conquer_Clip_Text_Print(List[index], x, y, LTGREY, TBLACK, TextFlags, width, Tabs);
            }
            break;
    }
}
