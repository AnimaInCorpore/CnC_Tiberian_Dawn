#pragma once

#include "legacy_compat.h"

class FlyClass {
public:
    FlyClass(void);

    void Fly_Speed(int speed, MPHType maximum);
    ImpactType Physics(COORDINATE& coord, DirType facing);
    MPHType Get_Speed(void) const { return SpeedAdd; }

    void Code_Pointers(void);
    void Decode_Pointers(void);

private:
    unsigned SpeedAccum;
    MPHType SpeedAdd;
};

