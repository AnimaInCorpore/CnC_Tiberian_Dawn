#pragma once

class HouseTypeClass {
public:
    HouseTypeClass();
    explicit HouseTypeClass(HousesType house);
    HouseTypeClass(HousesType house,
                   char const* ini,
                   int fullname,
                   char const* ext,
                   int lemon,
                   int color,
                   int bright_color,
                   PlayerColorType remapcolor,
                   unsigned char const* remap,
                   char prefix);

    static void One_Time(void);
    static HousesType From_Name(char const* name);
    static HouseTypeClass const& As_Reference(HousesType house);
    static HouseTypeClass const* const Pointers[HOUSE_COUNT];

    unsigned char const* RemapTable;
    PlayerColorType RemapColor;
    HousesType House;
    char const* IniName;
    int FullName;
    char Suffix[4];
    int Lemon;
    int Color;
    int BrightColor;
    char Prefix;
};

class HouseClass {
public:
    struct TypeRef : public HouseTypeClass {
        explicit TypeRef(HousesType house = HOUSE_NONE);

        HouseTypeClass* operator->();
        HouseTypeClass const* operator->() const;
    };

    explicit HouseClass(HousesType house = HOUSE_NONE);

    long Available_Money(void) const;
    void Spend_Money(long amount);
    void Refund_Money(long amount);
    int Power_Fraction(void) const;

    bool Can_Build(AircraftType, int) const;
    bool Can_Build(StructType, int) const;
    const unsigned char* Remap_Table(bool = false, bool = false) const;

    static HouseClass* As_Pointer(HousesType house);

    TypeRef Class;
    long Money;
    bool IsHuman;
    int AircraftFactories;
    int InfantryFactories;
    int UnitFactories;
    int BuildingFactories;
    int PowerFraction;
};
