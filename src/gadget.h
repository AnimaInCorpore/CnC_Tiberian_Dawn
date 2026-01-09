#pragma once

#include "legacy_compat.h"

class ControlClass;

class GadgetClass {
public:
    typedef enum FlagEnum {
        LEFTPRESS = 0x0001,
        LEFTHELD = 0x0002,
        LEFTRELEASE = 0x0004,
        LEFTUP = 0x0008,
        RIGHTPRESS = 0x0010,
        RIGHTHELD = 0x0020,
        RIGHTRELEASE = 0x0040,
        RIGHTUP = 0x0080,
        KEYBOARD = 0x0100
    } FlagEnum;

    GadgetClass(int x, int y, int w, int h, unsigned flags, int sticky = false)
        : X(x),
          Y(y),
          Width(w),
          Height(h),
          Tail(NULL),
          IsSticky(sticky ? 1u : 0u),
          IsDisabled(0u),
          Flags(flags) {}

    virtual ~GadgetClass() {}

    void Add_Tail(GadgetClass& gadget) {
        GadgetClass* current = this;
        while (current->Tail) {
            current = current->Tail;
        }
        current->Tail = &gadget;
    }

    virtual KeyNumType Input(void) { return KN_NONE; }

    virtual void Draw_All(bool forced = true) {
        Draw_Me(forced ? 1 : 0);
        if (Tail) {
            Tail->Draw_All(forced);
        }
    }

    virtual void Flag_To_Redraw(void) {}

    virtual void Peer_To_Peer(unsigned, KeyNumType&, ControlClass&) {}

    virtual unsigned Get_ID(void) const { return 0; }

    virtual int Draw_Me(int forced = false) {
        (void)forced;
        return 0;
    }

    int X;
    int Y;
    int Width;
    int Height;

protected:
    virtual int Action(unsigned flags, KeyNumType& key) {
        (void)flags;
        (void)key;
        return 0;
    }
    GadgetClass* Tail;

    unsigned IsSticky : 1;
    unsigned IsDisabled : 1;
    unsigned Flags;
};

class TextButtonClass : public GadgetClass {
public:
    TextButtonClass(int id, int text, TextPrintType flags, int x, int y, int w)
        : GadgetClass(x, y, w, FontHeight + FontYSpacing + 2, 0u, false),
          Id(id),
          Text(text),
          TextFlags(flags),
          IsOn(false),
          NeedsRedraw(true) {}

    void Turn_On() { IsOn = true; }
    void Turn_Off() { IsOn = false; }
    void Flag_To_Redraw() { NeedsRedraw = true; }

    KeyNumType Input() { return KN_NONE; }

    void Draw_All(bool forced = true) {
        (void)forced;
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

class EditClass : public GadgetClass {
public:
    EditClass(int id, char* buffer, int max_length, TextPrintType flags, int x, int y, int w)
        : GadgetClass(x, y, w, FontHeight + FontYSpacing + 2, 0u, false),
          Id(id),
          Buffer(buffer),
          MaxLength(max_length),
          TextFlags(flags),
          HasFocus(false) {}

    void Set_Focus() { HasFocus = true; }

    KeyNumType Input(void) { return KN_NONE; }

    void Draw_All(bool forced = true) {
        (void)forced;
        (void)Id;
        (void)Buffer;
        (void)MaxLength;
        (void)TextFlags;
    }

private:
    int Id;
    char* Buffer;
    int MaxLength;
    TextPrintType TextFlags;
    bool HasFocus;
};
