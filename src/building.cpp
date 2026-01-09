#include "legacy_compat.h"

int Get_Build_Frame_Count(void const*) { return 0; }

BuildingClass::BuildingClass()
    : House(NULL), Mission(MISSION_NONE), ActLike(0), Class(NULL), IsLeader(false) {}

BuildingClass::BuildingClass(StructType, HousesType owner)
    : House(HouseClass::As_Pointer(owner)),
      Mission(MISSION_NONE),
      ActLike(0),
      Class(NULL),
      IsLeader(false) {}
