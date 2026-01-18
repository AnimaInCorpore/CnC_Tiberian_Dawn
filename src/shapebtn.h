#pragma once

#include "toggle.h"

class ShapeButtonClass : public ToggleClass {
public:
    ShapeButtonClass(void);
    ShapeButtonClass(unsigned id, void const* shapes, int x, int y);

    virtual int Draw_Me(int forced = false);
    virtual void Set_Shape(void const* data);

    enum ShapeButtonClassEnums {
        UP_SHAPE,
        DOWN_SHAPE,
        DISABLED_SHAPE
    };

    unsigned ReflectButtonState : 1;

protected:
    void const* ShapeData;
};

