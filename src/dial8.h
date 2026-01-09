#pragma once

#include "control.h"

class Dial8Class : public ControlClass
{
public:
    Dial8Class(int id, int x, int y, int w, int h, DirType dir);

    DirType Get_Direction(void) const;
    void Set_Direction(DirType dir);

    virtual int Draw_Me(int forced = false);

protected:
    virtual int Action(unsigned flags, KeyNumType& key);

private:
    int FaceX;
    int FaceY;
    int FacePoint[8][2];
    int FaceLine[8][2];
    DirType Direction;
    FacingType Facing;
    FacingType OldFacing;
};

