#include "drive.h"

DriveClass::TurnTrackType const DriveClass::TrackControl[67] = { { 0, 0, 0, F_ } };
DriveClass::RawTrackType const DriveClass::RawTracks[13] = { { NULL, 0, 0, 0 } };

DriveClass::TrackType const DriveClass::Track13[] = { { 0, 0 } };
DriveClass::TrackType const DriveClass::Track12[] = { { 0, 0 } };
DriveClass::TrackType const DriveClass::Track11[] = { { 0, 0 } };
DriveClass::TrackType const DriveClass::Track10[] = { { 0, 0 } };
DriveClass::TrackType const DriveClass::Track9[] = { { 0, 0 } };
DriveClass::TrackType const DriveClass::Track8[] = { { 0, 0 } };
DriveClass::TrackType const DriveClass::Track7[] = { { 0, 0 } };
DriveClass::TrackType const DriveClass::Track6[] = { { 0, 0 } };
DriveClass::TrackType const DriveClass::Track5[] = { { 0, 0 } };
DriveClass::TrackType const DriveClass::Track4[] = { { 0, 0 } };
DriveClass::TrackType const DriveClass::Track3[] = { { 0, 0 } };
DriveClass::TrackType const DriveClass::Track2[] = { { 0, 0 } };
DriveClass::TrackType const DriveClass::Track1[24] = { { 0, 0 } };

DriveClass::DriveClass(void)
    : Class(&UnitTypeClass::As_Reference(UNIT_HARVESTER)),
      Tiberium(0),
      IsHarvesting(0),
      IsReturning(0),
      IsTurretLockedDown(0),
      IsOnShortTrack(0),
      SpeedAccum(0),
      TrackNumber(0),
      TrackIndex(0)
{
}

DriveClass::DriveClass(UnitType classid, HousesType house)
    : Class(&UnitTypeClass::As_Reference(classid)),
      Tiberium(0),
      IsHarvesting(0),
      IsReturning(0),
      IsTurretLockedDown(0),
      IsOnShortTrack(0),
      SpeedAccum(0),
      TrackNumber(0),
      TrackIndex(0)
{
    (void)house;
}

int DriveClass::Offload_Tiberium_Bail(void) { return 0; }

void DriveClass::Do_Turn(DirType dir) { (void)dir; }

void DriveClass::Approach_Target(void) {}

ObjectTypeClass const& DriveClass::Class_Of(void) const
{
    static ObjectTypeClass dummy(false, false, false, false, false, false, false, false, 0, "Drive", ARMOR_NONE, 0);
    return dummy;
}

void DriveClass::Overrun_Square(CELL cell, bool threaten)
{
    (void)cell;
    (void)threaten;
}

void DriveClass::Assign_Destination(TARGET target) { (void)target; }

void DriveClass::Per_Cell_Process(bool center) { (void)center; }

bool DriveClass::Ok_To_Move(DirType) const { return true; }

void DriveClass::AI(void) {}

#ifdef CHEAT_KEYS
void DriveClass::Debug_Dump(MonoClass* mono) const { (void)mono; }
#endif

void DriveClass::Force_Track(int track, COORDINATE coord)
{
    (void)track;
    (void)coord;
}

int DriveClass::Tiberium_Load(void) const
{
    if (*this == UNIT_HARVESTER) {
        return Cardinal_To_Fixed(UnitTypeClass::STEP_COUNT, Tiberium);
    }
    return 0x0000;
}

void DriveClass::Exit_Map(void) {}

void DriveClass::Mark_Track(COORDINATE headto, MarkType type)
{
    (void)headto;
    (void)type;
}

void DriveClass::Code_Pointers(void) {}

void DriveClass::Decode_Pointers(void) {}

void DriveClass::Fixup_Path(PathType* path) { (void)path; }

bool DriveClass::While_Moving(void) { return false; }

bool DriveClass::Start_Of_Move(void) { return false; }

void DriveClass::Lay_Track(void) {}

COORDINATE DriveClass::Smooth_Turn(COORDINATE adj, DirType* dir)
{
    (void)dir;
    return adj;
}

