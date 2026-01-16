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
          Next(NULL),
          Prev(NULL),
          IsSticky(sticky ? 1u : 0u),
          IsDisabled(0u),
          IsToRepaint(1u),
          Flags(flags) {}

    GadgetClass()
        : X(0),
          Y(0),
          Width(0),
          Height(0),
          Next(NULL),
          Prev(NULL),
          IsSticky(0u),
          IsDisabled(0u),
          IsToRepaint(1u),
          Flags(0u) {}

    virtual ~GadgetClass() { Clear_Focus(); }

    virtual KeyNumType Input(void);
    virtual void Draw_All(bool forced = true);
    virtual void Delete_List(void);
    virtual ControlClass* Extract_Gadget(unsigned id);
    virtual void Flag_List_To_Redraw(void) { LastList = 0; }
    virtual GadgetClass* Remove(void);
    virtual GadgetClass* Get_Next(void) const { return Next; }
    virtual GadgetClass* Get_Prev(void) const { return Prev; }

    virtual void Disable(void);
    virtual void Enable(void);
    virtual unsigned Get_ID(void) const { return 0; }
    virtual void Flag_To_Redraw(void);
    virtual void Peer_To_Peer(unsigned, KeyNumType&, ControlClass&) {}
    virtual void Set_Focus(void);
    virtual void Clear_Focus(void);
    virtual bool Has_Focus(void);
    virtual int Is_List_To_Redraw(void);

    virtual int Draw_Me(int forced = false);

    void Add_Tail(GadgetClass& gadget);

    int X;
    int Y;
    int Width;
    int Height;

protected:
    virtual void Sticky_Process(unsigned flags);
    virtual int Action(unsigned flags, KeyNumType& key) {
        if (flags) {
            IsToRepaint = 1u;
            Sticky_Process(flags);
            return 1;
        }
        return 0;
    }

protected:
    GadgetClass* Next;
    GadgetClass* Prev;

    /*
    ** Legacy UI gadgets (notably sliders/gauges) check the current sticky gadget
    ** while dragging; keep this available to derived classes.
    */
    static GadgetClass* StuckOn;

    unsigned IsSticky : 1;
    unsigned IsDisabled : 1;
    unsigned IsToRepaint : 1;
    unsigned Flags;

private:
    static GadgetClass* LastList;
    static GadgetClass* Focused;
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

    KeyNumType Input();
    void Draw_All(bool forced = true);

private:
    int Id;
    int Text;
    TextPrintType TextFlags;
    bool IsOn;
    bool NeedsRedraw;
};
