#include "function.h"
#include "facing.h"

FacingClass::FacingClass(void) {
    CurrentFacing = DIR_N;
    DesiredFacing = DIR_N;
}

int FacingClass::Set_Desired(DirType facing) {
    if (DesiredFacing != facing) {
        DesiredFacing = facing;
        return true;
    }
    return false;
}

int FacingClass::Set_Current(DirType facing) {
    if (CurrentFacing != facing) {
        CurrentFacing = facing;
        return true;
    }
    return false;
}

int FacingClass::Rotation_Adjust(int rate) {
    if (Is_Rotating()) {
        if (rate > 127) rate = 127;

        DirType oldfacing = CurrentFacing;
        int diff = Difference();

        if (ABS(diff) < rate) {
            CurrentFacing = DesiredFacing;
        } else {
            if (diff < 0) {
                CurrentFacing = static_cast<DirType>(CurrentFacing - static_cast<DirType>(rate));
            } else {
                CurrentFacing = static_cast<DirType>(CurrentFacing + static_cast<DirType>(rate));
            }
        }

        return (Facing_To_32(CurrentFacing) != Facing_To_32(oldfacing));
    }
    return false;
}

