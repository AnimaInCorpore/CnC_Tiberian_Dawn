#pragma once

#include "list.h"

class ColorListClass : public ListClass {
public:
    typedef enum SelectEnum {
        SELECT_NONE,
        SELECT_HIGHLIGHT,
        SELECT_BOX,
        SELECT_BAR,
        SELECT_INVERT,
    } SelectStyleType;

    ColorListClass(int id, int x, int y, int w, int h, TextPrintType flags, void const* up, void const* down);
    virtual ~ColorListClass(void);

    virtual int Add_Item(char const* text, char color = WHITE);
    virtual int Add_Item(int text, char color = WHITE);
    virtual void Remove_Item(char const* text);

    virtual void Set_Selected_Style(SelectStyleType style, int color = -1);

    DynamicVectorClass<char> Colors;

protected:
    virtual void Draw_Entry(int index, int x, int y, int width, int selected);

    SelectStyleType Style;
    int SelectColor;
};

