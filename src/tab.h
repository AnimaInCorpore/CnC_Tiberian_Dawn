#pragma once

#include "sidebar.h"
#include "credits.h"

class TabClass : public SidebarClass {
public:
    TabClass(void);
    virtual ~TabClass(void) {}

    virtual void AI(KeyNumType& input, int x, int y);
    virtual void Draw_It(bool complete = false);

    virtual void One_Time(void);
    static void Draw_Credits_Tab(void);
    static void Hilite_Tab(int tab);
    void Redraw_Tab(void) {
        IsToRedraw = true;
        Flag_To_Redraw(false);
    };

    virtual void Code_Pointers(void) {}
    virtual void Decode_Pointers(void) {}
    inline int Get_Tab_Height(void) { return (Tab_Height); };

    CreditClass Credits;

protected:
    unsigned IsToRedraw : 1;
    int Eva_Width;
    int Tab_Height;

private:
    void Set_Active(int select);

    static void const* TabShape;
};
