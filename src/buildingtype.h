#pragma once

// Building helpers used by bdata.cpp.
enum SmudgeType {
    SMUDGE_NONE = -1,
    SMUDGE_BIB1,
    SMUDGE_BIB2,
    SMUDGE_BIB3
};

class SmudgeTypeClass {
public:
    static SmudgeTypeClass const& As_Reference(SmudgeType);
    short const* Occupy_List() const;
    short const* Overlap_List() const;
};

enum BStateType {
    BSTATE_CONSTRUCTION,
    BSTATE_IDLE,
    BSTATE_ACTIVE,
    BSTATE_AUX1,
    BSTATE_AUX2,
    BSTATE_FULL,
    BSTATE_COUNT
};

enum BSizeType {
    BSIZE_11,
    BSIZE_21,
    BSIZE_12,
    BSIZE_22,
    BSIZE_23,
    BSIZE_32,
    BSIZE_33,
    BSIZE_42,
    BSIZE_55,
    BSIZE_COUNT
};

class BuildingTypeClass : public TechnoTypeClass {
public:
    BuildingTypeClass(StructType type,
                      int name,
                      char const* ininame,
                      COORDINATE exitpoint,
                      unsigned char level,
                      long pre,
                      bool is_scanner,
                      bool is_regulated,
                      bool is_bibbed,
                      bool is_nominal,
                      bool is_wall,
                      bool is_factory,
                      bool is_captureable,
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
                      short const* exitlist,
                      short const* sizelist,
                      short const* overlap);

    operator StructType() const { return Type; }

    static void One_Time(void);
    static void Init(TheaterType theater);
    static StructType From_Name(char const* name);
    static BuildingTypeClass const& As_Reference(StructType type);

    bool Create_And_Place(CELL cell, HousesType house) const;
    ObjectClass* Create_One_Of(HouseClass* house) const;
    void Init_Anim(BStateType state, int start, int count, int rate) const;
    int Legal_Placement(CELL pos) const;
    BuildingClass* Who_Can_Build_Me(bool intheory, bool legal, HousesType house) const;
    void Dimensions(int& width, int& height) const;
    short const* Occupy_List(bool placement = false) const;
    short const* Overlap_List(void) const;
    int Width(void) const;
    int Height(void) const;
    int Repair_Cost(void) const;
    int Repair_Step(void) const;
    bool Bib_And_Offset(SmudgeType& bib, CELL& cell) const;
    int Max_Pips(void) const;
    int Full_Name(void) const;
    int Raw_Cost(void) const;
    int Cost_Of(void) const;

    unsigned long CanEnter;
    unsigned Capacity;
    int Drain;
    short const* ExitList;
    COORDINATE ExitPoint;
    bool IsBibbed;
    bool IsCaptureable;
    bool IsFactory;
    bool IsRegulated;
    bool IsSimpleDamage;
    bool IsSturdy;
    bool IsWall;
    short const* OccupyList;
    short const* OverlapList;
    int Power;
    BSizeType Size;
    DirType StartFace;
    RTTIType ToBuild;
    StructType Type;
    void const* BuildupData;

    struct AnimControl {
        int Start;
        int Count;
        int Rate;
    };

    AnimControl Anims[BSTATE_COUNT];

    static BuildingTypeClass const* const Pointers[STRUCT_COUNT];
};

