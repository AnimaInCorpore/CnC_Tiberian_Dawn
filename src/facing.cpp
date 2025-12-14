/*
**	Command & Conquer(tm)
**	Copyright 2025 Electronic Arts Inc.
*/

#include "legacy/function.h"
#include "facing.h"

FacingClass::FacingClass(void)
{
    CurrentFacing = DIR_N;
    DesiredFacing = DIR_N;
}

int FacingClass::Set_Desired(DirType facing)
{
    if (DesiredFacing != facing) {
        DesiredFacing = facing;
        return(true);
    }
    return(false);
}

int FacingClass::Set_Current(DirType facing)
{
    if (CurrentFacing != facing) {
        CurrentFacing = facing;
        return(true);
    }
    return(false);
}

int FacingClass::Rotation_Adjust(int rate)
{
    if (Is_Rotating()) {
        rate = MIN(rate, 127);

        DirType oldfacing = CurrentFacing;
        int diff = Difference();

        if (ABS(diff) < rate) {
            CurrentFacing = DesiredFacing;
        } else {
            if (diff < 0) {
                CurrentFacing = (DirType)(CurrentFacing - (DirType)rate);
            } else {
                CurrentFacing = (DirType)(CurrentFacing + (DirType)rate);
            }
        }

        return(Facing_To_32(CurrentFacing) != Facing_To_32(oldfacing));
    }
    return(false);
}
