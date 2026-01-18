#pragma once

#include "gadget.h"

class TextLabelClass : public GadgetClass {
public:
    TextLabelClass(char* txt, int x, int y, int color, TextPrintType style);

    virtual int Draw_Me(int forced = false);
    virtual void Set_Text(char* txt) { Text = txt; }

    unsigned long UserData;
    TextPrintType Style;
    char* Text;
    int Color;
    int PixWidth;
    char Segments;
    unsigned short CRC;
};

