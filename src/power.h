#pragma once

#include "radar.h"

class PowerClass : public RadarClass {
public:
    int PowX;
    int PowY;
    int PowWidth;
    int PowHeight;
    int PowLineSpace;
    int PowLineWidth;

    PowerClass() : PowX(0), PowY(0), PowWidth(0), PowHeight(0), PowLineSpace(0), PowLineWidth(0) {}
    virtual ~PowerClass() {}

    virtual void Init_Clear(void) {}
    virtual void Draw_It(bool complete = false) { (void)complete; }
    virtual void AI(KeyNumType& input, int x, int y) {
        (void)input;
        (void)x;
        (void)y;
    }
};
