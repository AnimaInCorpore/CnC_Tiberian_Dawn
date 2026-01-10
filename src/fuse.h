#pragma once

#include "legacy_compat.h"

class FuseClass {
public:
    FuseClass(void);

    void Arm_Fuse(COORDINATE location, COORDINATE target, int time = 0xFF, int arming = 0);
    bool Fuse_Checkup(COORDINATE newlocation);
    void Fuse_Write(FileClass& file);
    void Fuse_Read(FileClass& file);
    COORDINATE Fuse_Target(void);

    void Code_Pointers(void);
    void Decode_Pointers(void);

    unsigned char Timer;

private:
    unsigned char Arming;
    COORDINATE HeadTo;
    short Proximity;
};

inline COORDINATE FuseClass::Fuse_Target(void) { return HeadTo; }

