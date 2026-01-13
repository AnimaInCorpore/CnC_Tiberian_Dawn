#pragma once

#include "display.h"

class RadarClass : public DisplayClass {
public:
    RadarClass() {}
    virtual ~RadarClass() {}

    virtual void Init_Clear(void) {}
    virtual void Draw_It(bool complete = false) { (void)complete; }
    virtual void AI(KeyNumType& input, int x, int y) {
        (void)input;
        (void)x;
        (void)y;
    }
};
