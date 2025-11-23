#pragma once

#include "mission.h"
#include "target.h"

class MapEditClass;
class HouseClass;

class WeaponTypeClass
{
public:
    BulletType Fires;
    unsigned char Attack;
    unsigned char ROF;
    int Range;
    VocType Sound;
    AnimType Anim;
};

class WarheadTypeClass
{
public:
    int SpreadFactor;
    bool IsWallDestroyer;
    bool IsWoodDestroyer;
    bool IsTiberiumDestroyer;
    unsigned char Modifier[ARMOR_COUNT];
};

class HouseTypeClass {
public:
    HousesType House;
    char const *IniName;
    int FullName;
    char Suffix[4];
    unsigned Lemon;
    unsigned char Color;
    unsigned char BrightColor;
    unsigned char const * RemapTable;
    PlayerColorType RemapColor;
    char Prefix;

    HouseTypeClass(HousesType house,
                        char const *ini,
                        int fullname,
                        char const *ext,
                        int lemon,
                        int color,
                        int bright_color,
                        PlayerColorType remapcolor,
                        unsigned char const * remap,
                        char prefix);

    static HousesType  From_Name(char const *name);
    static HouseTypeClass const &  As_Reference(HousesType house);
    static void One_Time(void);

private:
    static HouseTypeClass const * const Pointers[HOUSE_COUNT];
};

class AbstractTypeClass
{
public:
    char IniName[9];
    int Name;

    AbstractTypeClass(void) {};
    AbstractTypeClass(int name, char const * ini);
    virtual RTTIType  What_Am_I(void) const;

    virtual COORDINATE  Coord_Fixup(COORDINATE coord) const;
    virtual int  Full_Name(void) const;
    void  Set_Name(char const *buf) const
    {
        strncpy((char *)IniName, buf, sizeof(IniName));
        ((char &)IniName[sizeof(IniName)-1]) = '\0';
    };
    virtual unsigned short  Get_Ownable(void) const;
};

class ObjectTypeClass : public AbstractTypeClass
{
public:
    unsigned IsCrushable:1;
    unsigned IsStealthy:1;
    unsigned IsSelectable:1;
    unsigned IsLegalTarget:1;
    unsigned IsInsignificant:1;
    unsigned IsImmune:1;
    unsigned IsFlammable:1;
    unsigned IsSentient:1;

    ArmorType Armor;
    unsigned short MaxStrength;
    void const * ImageData;
    void const * RadarIcon;

    ObjectTypeClass(	bool is_sentient,
                            bool is_flammable,
                            bool is_crushable,
                            bool is_stealthy,
                            bool is_selectable,
                            bool is_legal_target,
                            bool is_insignificant,
                            bool is_immune,
                            int fullname,
                            char const *name,
                            ArmorType armor,
                            unsigned short strength);

    static void One_Time(void);

    virtual int  Max_Pips(void) const;
    virtual void  Dimensions(int &width, int &height) const;
    virtual bool  Create_And_Place(CELL , HousesType =HOUSE_NONE) const = 0;
    virtual int  Cost_Of(void) const;
    virtual int  Time_To_Build(HousesType house) const;
    virtual ObjectClass *  Create_One_Of(HouseClass *) const = 0;
    virtual short const *  Occupy_List(bool placement=false) const;
    virtual short const *  Overlap_List(void) const;
    virtual BuildingClass *  Who_Can_Build_Me(bool, bool, HousesType) const;
    virtual void const *  Get_Cameo_Data(void) const;
    void const *  Get_Image_Data(void) const {return ImageData;};
    void const *  Get_Radar_Data(void) const {return RadarIcon;};

    #ifdef SCENARIO_EDITOR
    virtual void  Display(int, int, WindowNumberType, HousesType) const {};
    #endif

    static void const * SelectShapes;
    static void const * PipShapes;
};

class TechnoTypeClass : public ObjectTypeClass
{
public:
    unsigned IsLeader:1;
    unsigned IsScanner:1;
    unsigned IsNominal:1;
    unsigned IsTheater:1;
    unsigned IsTurretEquipped:1;
    unsigned IsTwoShooter:1;
    unsigned IsRepairable:1;
    unsigned IsBuildable:1;
    unsigned IsCrew:1;
    unsigned IsTransporter:1;

    int SightRange;
    int Cost;
    unsigned char Scenario;
    unsigned char Level;
    long Pre;
    int Risk,Reward;
    MPHType MaxSpeed;
    int MaxAmmo;
    unsigned short Ownable;
    void const * CameoData;
    WeaponType Primary;
    WeaponType Secondary;

    TechnoTypeClass(
            int name,
            char const *ininame,
            unsigned char level,
            long pre,
            bool is_leader,
            bool is_scanner,
            bool is_nominal,
            bool is_transporter,
            bool is_flammable,
            bool is_crushable,
            bool is_stealthy,
            bool is_selectable,
            bool is_legal_target,
            bool is_insignificant,
            bool is_immune,
            bool is_theater,
            bool is_twoshooter,
            bool is_turret_equipped,
            bool is_repairable,
            bool is_buildable,
            bool is_crew,
            int ammo,
            unsigned short strength,
            MPHType maxspeed,
            int sightrange,
            int cost,
            int scenario,
            int risk,
            int reward,
            int ownable,
            WeaponType primary,
            WeaponType secondary,
            ArmorType armor);

    virtual int  Raw_Cost(void) const;
    virtual int  Max_Passengers(void) const;
    virtual int  Repair_Cost(void) const;
    virtual int  Repair_Step(void) const;
    virtual void const *  Get_Cameo_Data(void) const;
    virtual int  Cost_Of(void) const;
    virtual int  Time_To_Build(HousesType house) const;
    virtual unsigned short  Get_Ownable(void) const;
};

class BuildingTypeClass : public TechnoTypeClass {
    enum BuildingTypeClassRepairEnums {
        REPAIR_PERCENT=102,		// 40% fixed point number.
        REPAIR_STEP=5				// Number of damage points recovered per "step".
    };

public:
    unsigned IsBibbed:1;
    unsigned IsWall:1;
    unsigned IsFactory:1;
    unsigned IsSimpleDamage:1;
    unsigned IsSturdy:1;
    unsigned IsCaptureable:1;
    unsigned IsRegulated:1;
    RTTIType ToBuild;
    COORDINATE ExitPoint;
    short const *ExitList;
    StructType Type;
    unsigned long CanEnter;
    DirType StartFace;
    unsigned Capacity;
    int Power;
    int Drain;
    BSizeType Size;
    typedef struct {
        int	Start;			// Starting frame of animation.
        int	Count;			// Number of frames in this animation.
        int	Rate;				// Number of ticks to delay between each frame.
    } AnimControlType;
    AnimControlType Anims[BSTATE_COUNT];

    BuildingTypeClass	(
                    StructType type,
                    int name,
                    char const *ininame,
                    COORDINATE exitpoint,
                    unsigned char level,
                    long pre,
                    bool is_scanner,
                    bool is_regulated,
                    bool is_bibbed,
                    bool is_nominal,
                    bool is_wall,
                    bool is_factory,
                    bool is_capturable,
                    bool is_flammable,
                    bool is_simpledamage,
                    bool is_stealthy,
                    bool is_selectable,
                    bool is_legal_target,
                    bool is_insignificant,
                    bool is_immune,
                    bool is_theater,
                    bool is_turret_equipped,
                    bool is_twoshooter,
                    bool is_repairable,
                    bool is_buildable,
                    bool is_crew,
                    bool is_sturdy,
                    RTTIType tobuild,
                    DirType sframe,
                    unsigned short strength,
                    int sightrange,
                    int cost,
                    int scenario,
                    int risk,
                    int reward,
                    int ownable,
                    WeaponType primary,
                    WeaponType secondary,
                    ArmorType armor,
                    unsigned long canenter,
                    unsigned capacity,
                    int power,
                    int drain,
                    BSizeType size,
                    short const *exitlist,
                    short const *sizelist,
                    short const *overlap);
    virtual RTTIType  What_Am_I(void) const {return RTTI_BUILDINGTYPE;};
    operator StructType(void) const {return(Type);};

    static BuildingTypeClass const &  As_Reference(StructType type);
    static StructType  From_Name(char const *name);
    static void  Init(TheaterType theater);
    static void One_Time(void);
    static void Prep_For_Add(void);

    int  Width(void) const;
    int  Height(void) const;

    virtual int  Cost_Of(void) const;
    virtual int  Full_Name(void) const;
    virtual COORDINATE  Coord_Fixup(COORDINATE coord) const {return coord & 0xFF00FF00L;}
    virtual int  Max_Pips(void) const;
    virtual void  Dimensions(int &width, int &height) const;
    virtual int  Legal_Placement(CELL pos) const;
    virtual bool  Create_And_Place(CELL cell, HousesType house) const;
    virtual ObjectClass *  Create_One_Of(HouseClass * house) const;
    virtual short const *  Occupy_List(bool placement=false) const;
    virtual short const *  Overlap_List(void) const;
    virtual BuildingClass *  Who_Can_Build_Me(bool intheory, bool legal, HousesType house) const;
    virtual void const *  Get_Buildup_Data(void) const {return(BuildupData);};

    virtual int  Raw_Cost(void) const;
    virtual int  Repair_Cost(void) const;
    virtual int  Repair_Step(void) const;
    bool  Bib_And_Offset(SmudgeType & bib, CELL & cell) const;

    #ifdef SCENARIO_EDITOR
    virtual void  Display(int x, int y, WindowNumberType window, HousesType house) const;
    #endif

private:
    short const *OccupyList;
    short const *OverlapList;
    static BuildingTypeClass const * const Pointers[STRUCT_COUNT];
    void const * BuildupData;
    void  Init_Anim(BStateType state, int start, int count, int rate) const;
};

class UnitTypeClass : public TechnoTypeClass
{
public:
    enum UnitTypeClassRepairEnums {
        TIBERIUM_STEP=25,					// Credits per step of Tiberium.
        STEP_COUNT=28,						// Number of steps a harvester can carry.
        FULL_LOAD_CREDITS=(TIBERIUM_STEP*STEP_COUNT),
        REPAIR_PERCENT=102,		// 40% fixed point number.
        REPAIR_STEP=4				// Number of damage points recovered per "step".
    };

    unsigned IsCrateGoodie:1;
    unsigned IsPieceOfEight:1;
    unsigned IsCrusher:1;
    unsigned IsToHarvest:1;
    unsigned IsChunkyShape:1;
    unsigned IsRadarEquipped:1;
    unsigned IsFireAnim:1;
    unsigned IsLockTurret:1;
    unsigned IsTracked:1;
    unsigned IsGigundo:1;
    unsigned IsCloakable:1;
    unsigned IsAnimating:1;
    UnitType Type;
    SpeedType Speed;
    unsigned char ROT;
    signed char TurretOffset;
    MissionType Mission;
    AnimType Explosion;
    int MaxSize;

    UnitTypeClass	(
                    UnitType type,
                    int name,
                    char const *ininame,
                    AnimType exp,
                    unsigned char level,
                    long pre,
                    bool is_goodie,
                    bool is_leader,
                    bool is_eight,
                    bool is_nominal,
                    bool is_transporter,
                    bool is_crushable,
                    bool is_crusher,
                    bool is_harvest,
                    bool is_stealthy,
                    bool is_selectable,
                    bool is_legal_target,
                    bool is_insignificant,
                    bool is_immune,
                    bool is_turret_equipped,
                    bool is_twoshooter,
                    bool is_repairable,
                    bool is_buildable,
                    bool is_crew,
                    bool is_radar_equipped,
                    bool is_fire_anim,
                    bool is_lock_turret,
                    bool is_tracked,
                    bool is_gigundo,
                    bool is_chunky,
                    bool is_cloakable,
                    bool is_animating,
                    int ammo,
                    unsigned short strength,
                    int sightrange,
                    int cost,
                    int scenario,
                    int risk,
                    int reward,
                    int ownable,
                    WeaponType primary,
                    WeaponType secondary,
                    ArmorType armor,
                    SpeedType speed,
                    MPHType maxSpeed,
                    unsigned rot,
                    int toffset,
                    MissionType order);
    virtual RTTIType  What_Am_I(void) const {return RTTI_UNITTYPE;};

    static UnitType  From_Name(char const *name);
    static UnitTypeClass const &  As_Reference(UnitType type);
    static void  Init(TheaterType );
    static void One_Time(void);
    static void Prep_For_Add(void);

    virtual void  Dimensions(int &width, int &height) const;
    virtual bool  Create_And_Place(CELL cell, HousesType house) const;
    virtual ObjectClass *  Create_One_Of(HouseClass * house) const;
    virtual short const *  Occupy_List(bool placement=false) const;
    virtual BuildingClass *  Who_Can_Build_Me(bool intheory, bool legal, HousesType house) const;
    virtual int  Max_Pips(void) const;

    virtual int  Repair_Cost(void) const;
    virtual int  Repair_Step(void) const;

    #ifdef SCENARIO_EDITOR
    virtual void  Display(int x, int y, WindowNumberType window, HousesType house) const;
    #endif

    static void const * WakeShapes;

private:
    static UnitTypeClass const * const Pointers[UNIT_COUNT];
};

class InfantryTypeClass : public TechnoTypeClass
{
private:
    static InfantryTypeClass const * const Pointers[INFANTRY_COUNT];

public:

    unsigned IsFemale:1;
    unsigned IsCrawling:1;
    unsigned IsCapture:1;
    unsigned IsFraidyCat:1;
    unsigned IsCivilian:1;
    InfantryType Type;
    DoInfoStruct DoControls[DO_COUNT];
    char FireLaunch;
    char ProneLaunch;

    InfantryTypeClass	(
                    InfantryType type,
                    int name,
                    char const *ininame,
                    unsigned char level,
                    long pre,
                    bool is_female,
                    bool is_leader,
                    bool is_crawling,
                    bool is_civilian,
                    bool is_nominal,
                    bool is_fraidycat,
                    bool is_capture,
                    bool is_theater,
                    int ammo,
                    int *do_table,
                    int firelaunch,
                    int pronelaunch,
                    unsigned short strength,
                    int sightrange,
                    int cost,
                    int scenario,
                    int risk,
                    int reward,
                    int ownable,
                    WeaponType primary,
                    WeaponType secondary,
                    MPHType maxSpeed);
    virtual RTTIType  What_Am_I(void) const {return RTTI_INFANTRYTYPE;};

    static InfantryType  From_Name(char const *name);
    static InfantryTypeClass const &  As_Reference(InfantryType type) {return *Pointers[type];};
    static void  Init(TheaterType );
    static void One_Time(void);
    static void Prep_For_Add(void);

    virtual void  Dimensions(int &width, int &height) const {width = 12;height = 16;};
    virtual bool  Create_And_Place(CELL cell, HousesType house) const;
    virtual ObjectClass *  Create_One_Of(HouseClass * house) const;
    virtual short const *  Occupy_List(bool placement=false) const;
    virtual BuildingClass *  Who_Can_Build_Me(bool intheory, bool legal, HousesType house) const;
    virtual int  Full_Name(void) const;

    #ifdef SCENARIO_EDITOR
    virtual void  Display(int x, int y, WindowNumberType window, HousesType house) const;
    #endif
};

class BulletTypeClass : public ObjectTypeClass
{
public:
    unsigned IsHigh:1;
    unsigned IsArcing:1;
    unsigned IsHoming:1;
    unsigned IsDropping:1;
    unsigned IsInvisible:1;
    unsigned IsProximityArmed:1;
    unsigned IsFlameEquipped:1;
    unsigned IsFueled:1;
    unsigned IsFaceless:1;
    unsigned IsInaccurate:1;
    unsigned IsTranslucent:1;
    unsigned IsAntiAircraft:1;
    BulletType Type;
    MPHType MaxSpeed;
    WarheadType Warhead;
    AnimType Explosion;
    unsigned char ROT;
    int Arming;
    int Range;

    BulletTypeClass(
        BulletType type,
        char const *ininame,
        bool is_high,
        bool is_homing,
        bool is_arcing,
        bool is_dropping,
        bool is_invisible,
        bool is_proximity_armed,
        bool is_flame_equipped,
        bool is_fueled,
        bool is_faceless,
        bool is_inaccurate,
        bool is_translucent,
        bool is_antiair,
        int arming,
        int range,
        MPHType maxspeed,
        unsigned rot,
        WarheadType warhead,
        AnimType explosion);

    virtual RTTIType  What_Am_I(void) const {return RTTI_BULLETTYPE;};

    static BulletTypeClass const &  As_Reference(BulletType type) {return *Pointers[type];};
    static void  Init(TheaterType ) {};
    static void One_Time(void);

    virtual bool  Create_And_Place(CELL , HousesType =HOUSE_NONE) const {return false;};
    virtual ObjectClass *  Create_One_Of(HouseClass *) const {return 0;};

private:
    static BulletTypeClass const * const Pointers[BULLET_COUNT];
};

class TerrainTypeClass : public ObjectTypeClass
{
public:
    TerrainType Type;
    unsigned IsDestroyable:1;
    unsigned IsTransformable:1;
    unsigned IsTiberiumSpawn:1;
    short FullName;
    COORDINATE CenterBase;
    unsigned char Theater;

    TerrainTypeClass(
            TerrainType terrain,
            int theater,
            COORDINATE centerbase,
            bool is_spawn,
            bool is_destroyable,
            bool is_transformable,
            bool is_flammable,
            bool is_crushable,
            bool is_selectable,
            bool is_legal_target,
            bool is_insignificant,
            bool is_immune,
            char const *ininame,
            int fullname,
            unsigned short strength,
            ArmorType armor,
            short const *occupy,
            short const *overlap);
    virtual RTTIType  What_Am_I(void) const {return RTTI_TERRAINTYPE;};

    static TerrainType  From_Name(char const*name);
    static TerrainTypeClass const &  As_Reference(TerrainType type) {return *Pointers[type];};
    static void  Init(TheaterType theater = THEATER_TEMPERATE);
    static void One_Time(void) {};
    static void Prep_For_Add(void);

    virtual COORDINATE  Coord_Fixup(COORDINATE coord) const {return coord & 0xFF00FF00L;}
    virtual bool  Create_And_Place(CELL cell, HousesType house) const;
    virtual ObjectClass *  Create_One_Of(HouseClass *) const;
    virtual short const *  Occupy_List(bool placement=false) const;
    virtual short const *  Overlap_List(void) const;

    #ifdef SCENARIO_EDITOR
    virtual void  Display(int x, int y, WindowNumberType window, HousesType house=HOUSE_NONE) const;
    #endif

private:
    short const *Occupy;
    short const *Overlap;

    static TerrainTypeClass const * const Pointers[TERRAIN_COUNT];
};

class TemplateTypeClass: public ObjectTypeClass
{
public:
    TemplateType Type;
    unsigned char Theater;
    LandType Land;
    int FullName;
    unsigned char Width,Height;
    LandType AltLand;
    char const *AltIcons;

    TemplateTypeClass(
        TemplateType iconset,
        int theater,
        char const *ininame,
        int fullname,
        LandType land,
        int width,
        int height,
        LandType altland,
        char const *alticons);
    virtual RTTIType  What_Am_I(void) const {return RTTI_TEMPLATETYPE;};

    static TemplateType  From_Name(char const *name);
    static TemplateTypeClass const &  As_Reference(TemplateType type) {return *Pointers[type];};
    static void  Init(TheaterType theater);
    static void One_Time(void);
    static void Prep_For_Add(void);

    virtual COORDINATE  Coord_Fixup(COORDINATE coord) const {return coord & 0xFF00FF00L;}
    virtual bool  Create_And_Place(CELL cell, HousesType house=HOUSE_NONE) const;
    virtual ObjectClass *  Create_One_Of(HouseClass *) const;
    virtual short const *  Occupy_List(bool placement=false) const;

    #ifdef SCENARIO_EDITOR
    virtual void  Display(int x, int y, WindowNumberType window, HousesType house=HOUSE_NONE) const;
    #endif

private:
    static TemplateTypeClass const * const Pointers[TEMPLATE_COUNT];
};

class AnimTypeClass : public ObjectTypeClass
{
public:
    unsigned IsNormalized:1;
    unsigned IsGroundLayer:1;
    unsigned IsTranslucent:1;
    unsigned IsWhiteTrans:1;
    unsigned IsFlameThrower:1;
    unsigned IsScorcher:1;
    unsigned IsCraterForming:1;
    unsigned IsSticky:1;
    AnimType Type;
    int Size;
    int Biggest;
    unsigned int Damage;
    unsigned char Delay;
    int Start;
    int LoopStart;
    int LoopEnd;
    int Stages;
    unsigned char Loops;
    VocType Sound;
    AnimType ChainTo;

    AnimTypeClass(AnimType anim,
                        char const *name,
                        int size,
                        int biggest,
                        bool isnormal,
                        bool iswhite,
                        bool isscorcher,
                        bool iscrater,
                        bool issticky,
                        bool ground,
                        bool istrans,
                        bool isflame,
                        unsigned int damage,
                        int delaytime,
                        int start,
                        int loopstart,
                        int loopend,
                        int stages,
                        int loops,
                        VocType sound,
                        AnimType chainto);
    virtual RTTIType  What_Am_I(void) const {return RTTI_ANIMTYPE;};

    static AnimTypeClass const &  As_Reference(AnimType type) {return *Pointers[type];};
    static void Init(TheaterType) {};
    static void One_Time(void);

    virtual bool  Create_And_Place(CELL , HousesType =HOUSE_NONE) const {return false;};
    virtual ObjectClass *  Create_One_Of(HouseClass *) const {return 0;};

private:
    static AnimTypeClass const * const Pointers[ANIM_COUNT];
};

class AircraftTypeClass : public TechnoTypeClass
{
    enum UnitTypeClassRepairEnums {
        REPAIR_PERCENT=102,		// 40% fixed point number.
        REPAIR_STEP=2				// Number of damage points recovered per "step".
    };
public:
    unsigned IsFixedWing:1;
    unsigned IsLandable:1;
    unsigned IsRotorEquipped:1;
    unsigned IsRotorCustom:1;

    AircraftType Type;
    unsigned char ROT;
    MissionType Mission;

    AircraftTypeClass(
            AircraftType airtype,
            int name,
            char const *ininame,
            unsigned char level,
            long pre,
            bool is_leader,
            bool is_twoshooter,
            bool is_transporter,
            bool is_fixedwing,
            bool is_rotorequipped,
            bool is_rotorcustom,
            bool is_landable,
            bool is_crushable,
            bool is_stealthy,
            bool is_selectable,
            bool is_legal_target,
            bool is_insignificant,
            bool is_immune,
            bool is_theater,
            bool is_repairable,
            bool is_buildable,
            bool is_crew,
            int ammo,
            unsigned short strength,
            int sightrange,
            int cost,
            int scenario,
            int risk,
            int reward,
            int ownable,
            WeaponType primary,
            WeaponType secondary,
            ArmorType armor,
            MPHType MaxSpeed,
            int ROT,
            MissionType deforder);
    virtual RTTIType  What_Am_I(void) const;

    static AircraftType  From_Name(char const *name);
    static AircraftTypeClass const &  As_Reference(AircraftType a) {return *Pointers[a];};
    static void  Init(TheaterType);
    static void One_Time(void);
    static void Prep_For_Add(void);

    virtual int  Repair_Cost(void) const;
    virtual int  Repair_Step(void) const;
    virtual void  Dimensions(int &width, int &height) const;
    virtual bool  Create_And_Place(CELL, HousesType) const;
    virtual ObjectClass *  Create_One_Of(HouseClass * house) const;
    virtual short const *  Occupy_List(bool placement=false) const;
    virtual short const *  Overlap_List(void) const;
    virtual BuildingClass *  Who_Can_Build_Me(bool intheory, bool legal, HousesType house) const;
    virtual int  Max_Pips(void) const;

    #ifdef SCENARIO_EDITOR
    virtual void  Display(int x, int y, WindowNumberType window, HousesType house) const;
    #endif

    static void const * LRotorData;
    static void const * RRotorData;

private:
    static AircraftTypeClass const * const Pointers[AIRCRAFT_COUNT];
};

class OverlayTypeClass: public ObjectTypeClass
{
public:
    OverlayType Type;
    int FullName;
    LandType Land;
    int DamageLevels;
    int DamagePoints;
    unsigned IsTheater:1;
    unsigned IsWall:1;
    unsigned IsHigh:1;
    unsigned IsTiberium:1;
    unsigned IsWooden:1;
    unsigned IsCrate:1;
    unsigned IsRadarVisible:1;

    OverlayTypeClass(
        OverlayType iconset,
        char const *ininame,
        int  fullname,
        LandType ground,
        int  damagelevels,
        int  damagepoints,
        bool isradarinvisible,
        bool iswooden,
        bool istarget,
        bool iscrushable,
        bool istiberium,
        bool high,
        bool theater,
        bool iswall,
        bool iscrate);
    virtual RTTIType  What_Am_I(void) const {return RTTI_OVERLAYTYPE;};

    static OverlayType  From_Name(char const *name);
    static OverlayTypeClass const &  As_Reference(OverlayType type) {return *Pointers[type];};
    static void  Init(TheaterType);
    static void One_Time(void);
    static void Prep_For_Add(void);

    virtual COORDINATE  Coord_Fixup(COORDINATE coord) const {return coord & 0xFF00FF00L;}
    virtual bool  Create_And_Place(CELL cell, HousesType house=HOUSE_NONE) const;
    virtual ObjectClass *  Create_One_Of(HouseClass *) const;
    virtual short const *  Occupy_List(bool placement=false) const;
    virtual void  Draw_It(int x, int y, int data) const;
    virtual unsigned char *  Radar_Icon(int data) const;

    #ifdef SCENARIO_EDITOR
    virtual void  Display(int x, int y, WindowNumberType window, HousesType house=HOUSE_NONE) const;
    #endif

private:
    static OverlayTypeClass const * const Pointers[OVERLAY_COUNT];
};

class SmudgeTypeClass : public ObjectTypeClass
{
public:
    SmudgeType Type;
    int FullName;
    int Width;
    int Height;
    unsigned IsCrater:1;
    unsigned IsBib:1;

    SmudgeTypeClass(
        SmudgeType smudge,
        char const *ininame,
        int fullname,
        int width,
        int height,
        bool isbib,
        bool iscrater
        );
    virtual RTTIType  What_Am_I(void) const {return RTTI_SMUDGETYPE;};

    static SmudgeType  From_Name(char const *name);
    static SmudgeTypeClass const &  As_Reference(SmudgeType type) {return *Pointers[type];};
    static void  Init(TheaterType);
    static void One_Time(void);
    static void Prep_For_Add(void);

    virtual bool  Create_And_Place(CELL cell, HousesType house=HOUSE_NONE) const;
    virtual ObjectClass *  Create_One_Of(HouseClass *) const;
    virtual short const *  Occupy_List(bool placement=false) const;
    virtual short const *  Overlap_List(void) const {return Occupy_List();};
    virtual void  Draw_It(int x, int y, int data) const ;

    #ifdef SCENARIO_EDITOR
    virtual void  Display(int x, int y, WindowNumberType window, HousesType house=HOUSE_NONE) const;
    #endif

private:
    static SmudgeTypeClass const * const Pointers[SMUDGE_COUNT];
};
