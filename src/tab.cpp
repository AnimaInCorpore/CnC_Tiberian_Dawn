#include "tab.h"

void const* TabClass::TabShape = NULL;

TabClass::TabClass(void) : IsToRedraw(false), Eva_Width(0), Tab_Height(0) {}

void TabClass::AI(KeyNumType& input, int x, int y) {
    (void)input;
    (void)x;
    (void)y;
}

void TabClass::Draw_It(bool complete) {
    (void)complete;
}

void TabClass::One_Time(void) {}

void TabClass::Draw_Credits_Tab(void) {}

void TabClass::Hilite_Tab(int tab) {
    (void)tab;
}

void TabClass::Set_Active(int select) {
    (void)select;
}
