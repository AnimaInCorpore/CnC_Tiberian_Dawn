#include "legacy/function.h"

TARGET TechnoType_To_Target(TechnoTypeClass const* ptr) {
  TARGET target;

  switch (ptr->What_Am_I()) {
    case RTTI_INFANTRYTYPE:
      target = Build_Target(KIND_INFANTRY, static_cast<const InfantryTypeClass*>(ptr)->Type);
      break;
    case RTTI_UNITTYPE:
      target = Build_Target(KIND_UNIT, static_cast<const UnitTypeClass*>(ptr)->Type);
      break;
    case RTTI_AIRCRAFTTYPE:
      target = Build_Target(KIND_AIRCRAFT, static_cast<const AircraftTypeClass*>(ptr)->Type);
      break;
    case RTTI_BUILDINGTYPE:
      target = Build_Target(KIND_BUILDING, static_cast<const BuildingTypeClass*>(ptr)->Type);
      break;
    default:
      target = 0;
      break;
  }

  return target;
}

TechnoTypeClass const* Target_To_TechnoType(TARGET target) {
  switch (Target_Kind(target)) {
    case KIND_INFANTRY:
      return &InfantryTypeClass::As_Reference(static_cast<InfantryType>(Target_Value(target)));
    case KIND_UNIT:
      return &UnitTypeClass::As_Reference(static_cast<UnitType>(Target_Value(target)));
    case KIND_AIRCRAFT:
      return &AircraftTypeClass::As_Reference(static_cast<AircraftType>(Target_Value(target)));
    case KIND_BUILDING:
      return &BuildingTypeClass::As_Reference(static_cast<StructType>(Target_Value(target)));
    default:
      break;
  }
  return nullptr;
}
