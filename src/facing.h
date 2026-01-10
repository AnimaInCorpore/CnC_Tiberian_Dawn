#pragma once

class FacingClass {
public:
    FacingClass(void);
    FacingClass(DirType dir) { CurrentFacing = DesiredFacing = dir; }
    operator DirType(void) const { return CurrentFacing; }

    DirType Current(void) const { return CurrentFacing; }
    DirType Desired(void) const { return DesiredFacing; }

    int Set_Desired(DirType facing);
    int Set_Current(DirType facing);

    void Set(DirType facing) {
        Set_Current(facing);
        Set_Desired(facing);
    }

    DirType Get(void) const { return CurrentFacing; }

    int Is_Rotating(void) const { return (DesiredFacing != CurrentFacing); }

    int Difference(void) const {
        unsigned char desired = static_cast<unsigned char>(DesiredFacing);
        unsigned char current = static_cast<unsigned char>(CurrentFacing);
        return static_cast<signed char>(desired - current);
    }

    int Difference(DirType facing) const {
        unsigned char desired = static_cast<unsigned char>(facing);
        unsigned char current = static_cast<unsigned char>(CurrentFacing);
        return static_cast<signed char>(desired - current);
    }

    int Rotation_Adjust(int rate);

private:
    DirType CurrentFacing;
    DirType DesiredFacing;
};

