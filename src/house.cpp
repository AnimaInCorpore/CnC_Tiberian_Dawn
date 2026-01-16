#include "legacy_compat.h"
#include "house.h"

HouseTypeClass::HouseTypeClass()
    : RemapTable(NULL),
      RemapColor(REMAP_NONE),
      House(HOUSE_NONE),
      IniName(NULL),
      FullName(0),
      Lemon(0),
      Color(0),
      BrightColor(0),
      Prefix('\0') {
    Suffix[0] = '\0';
}

HouseTypeClass::HouseTypeClass(HousesType house)
    : RemapTable(NULL),
      RemapColor(REMAP_NONE),
      House(house),
      IniName(NULL),
      FullName(0),
      Lemon(0),
      Color(0),
      BrightColor(0),
      Prefix('\0') {
    Suffix[0] = '\0';
}

HouseClass::TypeRef::TypeRef(HousesType house)
    : HouseTypeClass(house)
{
}

HouseTypeClass* HouseClass::TypeRef::operator->() { return this; }
HouseTypeClass const* HouseClass::TypeRef::operator->() const { return this; }

HouseClass::HouseClass(HousesType house)
    : Class(house),
      Money(0),
      IsHuman(false),
      AircraftFactories(1),
      InfantryFactories(1),
      UnitFactories(1),
      BuildingFactories(1),
      PowerFraction(0x0100)
{
}

long HouseClass::Available_Money(void) const { return Money; }

void HouseClass::Spend_Money(long amount)
{
    if (amount <= 0) return;
    if (Money > amount) {
        Money -= amount;
    } else {
        Money = 0;
    }
}

void HouseClass::Refund_Money(long amount)
{
    if (amount <= 0) return;
    Money += amount;
}

int HouseClass::Power_Fraction(void) const { return PowerFraction; }

bool HouseClass::Can_Build(AircraftType, int) const { return true; }
bool HouseClass::Can_Build(StructType, int) const { return true; }

const unsigned char* HouseClass::Remap_Table(bool, bool) const { return NULL; }

HouseClass* HouseClass::As_Pointer(HousesType house)
{
    static HouseClass houses[HOUSE_COUNT + 1];
    static bool initialized = false;
    if (!initialized) {
        for (int i = 0; i < HOUSE_COUNT + 1; ++i) {
            houses[i] = HouseClass(static_cast<HousesType>(i - 1));
        }
        initialized = true;
    }
    int index = static_cast<int>(house) + 1;
    if (index < 0 || index >= HOUSE_COUNT + 1) return NULL;
    return &houses[index];
}
