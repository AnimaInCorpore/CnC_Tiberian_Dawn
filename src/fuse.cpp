/* Ported from FUSE.CPP */
#include "legacy/function.h"

FuseClass::FuseClass(void)
{
    Timer = 0;
    Arming = 0;
    HeadTo = 0;
    Proximity = 0;
}

void FuseClass::Arm_Fuse(COORDINATE location, COORDINATE target, int timeto, int arming)
{
    timeto = MAX(timeto, arming);
    Timer = MIN(timeto, 0xFF);
    Arming = MIN(arming, 0xFF);
    HeadTo = target;
    Proximity = Distance(location, target);
}

bool FuseClass::Fuse_Checkup(COORDINATE newlocation)
{
    int proximity;

    if (Timer) Timer--;

    if (Arming) {
        Arming--;
    } else {
        if (!Timer) return true;

        proximity = Distance(newlocation, HeadTo);
        if (proximity < 0x0010) return true;
        if (proximity < ICON_LEPTON_W && proximity > Proximity) {
            return true;
        }
        Proximity = proximity;
    }
    return false;
}

void FuseClass::Fuse_Write(FileClass & file)
{
    file.Write(&Timer, sizeof(Timer));
    file.Write(&Arming, sizeof(Arming));
    file.Write(&HeadTo, sizeof(HeadTo));
    file.Write(&Proximity, sizeof(Proximity));
}

void FuseClass::Fuse_Read(FileClass & file)
{
    file.Read(&Timer, sizeof(Timer));
    file.Read(&Arming, sizeof(Arming));
    file.Read(&HeadTo, sizeof(HeadTo));
    file.Read(&Proximity, sizeof(Proximity));
}
