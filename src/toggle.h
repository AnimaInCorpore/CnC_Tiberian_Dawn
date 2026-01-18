#pragma once

#include "control.h"

/*
** ToggleClass implements Windows-style "sticky" buttons: click to press,
** capture while held, and activate on release when still over the gadget.
*/
class ToggleClass : public ControlClass {
public:
    ToggleClass(unsigned id, int x, int y, int w, int h);

    virtual void Turn_On(void);
    virtual void Turn_Off(void);

    unsigned IsPressed : 1;
    unsigned IsOn : 1;
    unsigned IsToggleType : 1;

protected:
    virtual int Action(unsigned flags, KeyNumType& key);
};

