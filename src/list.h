#pragma once

#include "control.h"
#include "shapebtn.h"
#include "slider.h"

class ListClass : public ControlClass {
public:
    ListClass(int id, int x, int y, int w, int h, TextPrintType flags, void const* up, void const* down);
    virtual ~ListClass(void);

    virtual int Add_Item(char const* text);
    virtual int Add_Item(int text);
    virtual int Add_Scroll_Bar(void);
    virtual void Bump(int up);
    virtual int Count(void) { return List.Count(); }
    virtual int Current_Index(void);
    virtual char const* Current_Item(void);
    virtual int Draw_Me(int forced = false);
    virtual char const* Get_Item(int index) const;
    virtual int Step_Selected_Index(int step);

    virtual void Peer_To_Peer(unsigned flags, KeyNumType& key, ControlClass& whom);
    virtual void Remove_Item(char const* text);
    virtual int Remove_Scroll_Bar(void);
    virtual void Set_Selected_Index(int index);
    virtual void Set_Tabs(int const* tabs);
    virtual int Set_View_Index(int index);
    virtual void Step(int up);

    virtual GadgetClass* Remove(void);

protected:
    virtual int Action(unsigned flags, KeyNumType& key);
    virtual void Draw_Entry(int index, int x, int y, int width, int selected);

    TextPrintType TextFlags;
    int const* Tabs;
    DynamicVectorClass<char*> List;
    int LineHeight;
    int LineCount;

    unsigned IsScrollActive : 1;
    ShapeButtonClass UpGadget;
    ShapeButtonClass DownGadget;
    SliderClass ScrollGadget;

    int SelectedIndex;
    int CurrentTopIndex;

private:
    static char* Duplicate_(char const* text);
    static void Free_(char* text);
};

