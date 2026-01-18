#pragma once

#include "toggle.h"

class TextButtonClass : public ToggleClass {
public:
    TextButtonClass(void);
    TextButtonClass(unsigned id,
                    char const* text,
                    TextPrintType style,
                    int x,
                    int y,
                    int w = -1,
                    int h = -1,
                    int blackborder = false);
    TextButtonClass(unsigned id, int text, TextPrintType style, int x, int y, int w = -1, int h = -1, int blackborder = false);

    virtual int Draw_Me(int forced = false);
    virtual void Set_Text(char const* text, bool resize = false);
    virtual void Set_Text(int text, bool resize = false);
    virtual void Set_Style(TextPrintType style) { PrintFlags = style; }

protected:
    virtual void Draw_Background(void);
    virtual void Draw_Text(char const* text);

    unsigned IsBlackBorder : 1;
    char const* String;
    TextPrintType PrintFlags;
};

