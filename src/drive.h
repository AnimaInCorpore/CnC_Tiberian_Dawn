#ifndef DRIVE_H
#define DRIVE_H

#include "legacy_compat.h"

struct PathType;
class MonoClass;

class DriveClass : public FootClass
{
public:
    UnitTypeClass const* const Class;

    unsigned char Tiberium;

    unsigned IsHarvesting : 1;
    unsigned IsReturning : 1;
    unsigned IsTurretLockedDown : 1;
    unsigned IsOnShortTrack : 1;

    DriveClass(void);
    DriveClass(UnitType classid, HousesType house);
    virtual ~DriveClass(void) {}
    operator UnitType(void) const { return Class->Type; }

    virtual int Offload_Tiberium_Bail(void);
    void Do_Turn(DirType dir);
    virtual void Approach_Target(void);
    virtual ObjectTypeClass const& Class_Of(void) const;
    virtual void Overrun_Square(CELL cell, bool threaten = true);
    virtual void Assign_Destination(TARGET target);
    virtual void Per_Cell_Process(bool center);
    virtual bool Ok_To_Move(DirType) const;
    virtual void AI(void);
#ifdef CHEAT_KEYS
    virtual void Debug_Dump(MonoClass* mono) const;
#endif
    void Force_Track(int track, COORDINATE coord);
    virtual int Tiberium_Load(void) const;

    void Exit_Map(void);
    void Mark_Track(COORDINATE headto, MarkType type);

    virtual void Code_Pointers(void);
    virtual void Decode_Pointers(void);

    enum DriveClassEnum {
        BACKUP_INTO_REFINERY = 64,
        OUT_OF_REFINERY,
        OUT_OF_WEAPON_FACTORY
    };

private:
    typedef enum TrackControlType {
        F_ = 0x00,
        F_T = 0x01,
        F_X = 0x02,
        F_Y = 0x04,
        F_D = 0x08
    } TrackControlType;

    friend TrackControlType operator|(TrackControlType a, TrackControlType b)
    {
        return static_cast<TrackControlType>(static_cast<int>(a) | static_cast<int>(b));
    }

    friend TrackControlType operator&(TrackControlType a, TrackControlType b)
    {
        return static_cast<TrackControlType>(static_cast<int>(a) & static_cast<int>(b));
    }

    friend TrackControlType operator~(TrackControlType a) { return static_cast<TrackControlType>(~static_cast<int>(a)); }

    typedef struct {
        char Track;
        char StartTrack;
        DirType Facing;
        DriveClass::TrackControlType Flag;
    } TurnTrackType;

    typedef struct {
        COORDINATE Offset;
        DirType Facing;
    } TrackType;

    typedef struct {
        DriveClass::TrackType const* Track;
        int Jump;
        int Entry;
        int Cell;
    } RawTrackType;

    unsigned char SpeedAccum;
    char TrackNumber;
    char TrackIndex;

    virtual void Fixup_Path(PathType* path);
    bool While_Moving(void);
    bool Start_Of_Move(void);
    void Lay_Track(void);
    COORDINATE Smooth_Turn(COORDINATE adj, DirType* dir);

    static TurnTrackType const TrackControl[67];
    static RawTrackType const RawTracks[13];
    static TrackType const Track13[];
    static TrackType const Track12[];
    static TrackType const Track11[];
    static TrackType const Track10[];
    static TrackType const Track9[];
    static TrackType const Track8[];
    static TrackType const Track7[];
    static TrackType const Track6[];
    static TrackType const Track5[];
    static TrackType const Track4[];
    static TrackType const Track3[];
    static TrackType const Track2[];
    static TrackType const Track1[24];
};

#endif
