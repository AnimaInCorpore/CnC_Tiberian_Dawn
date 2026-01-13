#pragma once

#include "power.h"
#include "function.h"

class SidebarClass : public PowerClass {
public:
    int SideX;
    int SideY;
    int SideBarWidth;
    int SideWidth;
    int SideHeight;
    int TopHeight;
    int MaxVisible;
    int ButtonOneWidth;
    int ButtonTwoWidth;
    int ButtonThreeWidth;
    int ButtonHeight;

    SidebarClass() : SideX(0), SideY(0), SideBarWidth(0), SideWidth(0), SideHeight(0), TopHeight(0), MaxVisible(0), ButtonOneWidth(0), ButtonTwoWidth(0), ButtonThreeWidth(0), ButtonHeight(0) {}
    virtual ~SidebarClass() {}

    enum SideBarClassEnums {
        BUTTON_ACTIVATOR = 100,
        SIDEBARWIDTH = 80
    };
};
