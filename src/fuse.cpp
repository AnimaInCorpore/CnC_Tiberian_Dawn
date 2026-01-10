#include "function.h"

#include "fuse.h"

FuseClass::FuseClass(void) : Timer(0), Arming(0), HeadTo(0), Proximity(0) {}

void FuseClass::Arm_Fuse(COORDINATE location, COORDINATE target, int timeto, int arming) {
    timeto = MAX(timeto, arming);
    Timer = static_cast<unsigned char>(MIN(timeto, 0xFF));
    Arming = static_cast<unsigned char>(MIN(arming, 0xFF));
    HeadTo = target;
    Proximity = static_cast<short>(Distance(location, target));
}

bool FuseClass::Fuse_Checkup(COORDINATE newlocation) {
    int proximity;

    if (Timer) {
        Timer--;
    }

    if (Arming) {
        Arming--;
    } else {
        if (!Timer) {
            return true;
        }

        proximity = Distance(newlocation, HeadTo);
        if (proximity < 0x0010) {
            return true;
        }
        if (proximity < ICON_LEPTON_W && proximity > Proximity) {
            return true;
        }
        Proximity = static_cast<short>(proximity);
    }
    return false;
}

void FuseClass::Fuse_Write(FileClass& file) {
    file.Write(&Timer, sizeof(Timer));
    file.Write(&Arming, sizeof(Arming));
    file.Write(&HeadTo, sizeof(HeadTo));
    file.Write(&Proximity, sizeof(Proximity));
}

void FuseClass::Fuse_Read(FileClass& file) {
    file.Read(&Timer, sizeof(Timer));
    file.Read(&Arming, sizeof(Arming));
    file.Read(&HeadTo, sizeof(HeadTo));
    file.Read(&Proximity, sizeof(Proximity));
}

void FuseClass::Code_Pointers(void) {}

void FuseClass::Decode_Pointers(void) {}

