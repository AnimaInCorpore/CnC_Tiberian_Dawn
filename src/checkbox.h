#pragma once

#include "toggle.h"

class CheckBoxClass : public ToggleClass {
public:
    CheckBoxClass(unsigned id, int x, int y) : ToggleClass(id, x, y, 7, 7) {}

    virtual int Draw_Me(int forced = false);
};

