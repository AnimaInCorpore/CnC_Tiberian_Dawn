#pragma once

#include "legacy_compat.h"

class GadgetClass {
public:
    enum GadgetFlags {
        LEFTPRESS = 1u << 0,
        RIGHTPRESS = 1u << 1
    };

    GadgetClass(int x, int y, int w, int h, unsigned flags)
        : X(x), Y(y), Width(w), Height(h), Flags(flags), Tail(NULL) {}

    virtual ~GadgetClass() {}

    void Add_Tail(GadgetClass& gadget) { Tail = &gadget; }

    virtual void Draw_All() {}

protected:
    int X;
    int Y;
    int Width;
    int Height;
    unsigned Flags;
    GadgetClass* Tail;
};

class ControlClass : public GadgetClass {
public:
    ControlClass(int id, int x, int y, int w, int h, unsigned flags)
        : GadgetClass(x, y, w, h, flags), Id(id) {}

private:
    int Id;
};

class TextButtonClass : public GadgetClass {
public:
    TextButtonClass(int id, int text, TextPrintType flags, int x, int y, int w)
        : GadgetClass(x, y, w, FontHeight + FontYSpacing + 2, 0u),
          Id(id),
          Text(text),
          TextFlags(flags),
          IsOn(false),
          NeedsRedraw(true) {}

    void Turn_On() { IsOn = true; }
    void Turn_Off() { IsOn = false; }
    void Flag_To_Redraw() { NeedsRedraw = true; }

    KeyNumType Input() { return KN_NONE; }

    void Draw_All() {
        NeedsRedraw = false;
        (void)Id;
        (void)Text;
        (void)TextFlags;
        (void)IsOn;
    }

private:
    int Id;
    int Text;
    TextPrintType TextFlags;
    bool IsOn;
    bool NeedsRedraw;
};

