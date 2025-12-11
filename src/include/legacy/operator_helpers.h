#pragma once

#include "control.h"
#include "defines.h"
#include "edit.h"
#include "gadget.h"

namespace legacy_operators {
template <typename T>
inline T OrEnum(T lhs, T rhs) {
  return static_cast<T>(static_cast<int>(lhs) | static_cast<int>(rhs));
}

template <typename T>
inline T AndEnum(T lhs, T rhs) {
  return static_cast<T>(static_cast<int>(lhs) & static_cast<int>(rhs));
}

template <typename T>
inline T NotEnum(T value) {
  return static_cast<T>(~static_cast<int>(value));
}

template <typename T>
inline T PostIncrementEnum(T& value) {
  const T old = value;
  value = static_cast<T>(static_cast<int>(value) + 1);
  return old;
}
}  // namespace legacy_operators

// Text print flag helpers.
inline TextPrintType operator|(TextPrintType lhs, TextPrintType rhs) {
  return legacy_operators::OrEnum(lhs, rhs);
}
inline TextPrintType operator&(TextPrintType lhs, TextPrintType rhs) {
  return legacy_operators::AndEnum(lhs, rhs);
}
inline TextPrintType operator~(TextPrintType value) {
  return legacy_operators::NotEnum(value);
}

// Edit style helpers.
inline EditClass::EditStyle operator|(EditClass::EditStyle lhs, EditClass::EditStyle rhs) {
  return legacy_operators::OrEnum(lhs, rhs);
}
inline EditClass::EditStyle operator&(EditClass::EditStyle lhs, EditClass::EditStyle rhs) {
  return legacy_operators::AndEnum(lhs, rhs);
}
inline EditClass::EditStyle operator~(EditClass::EditStyle value) {
  return legacy_operators::NotEnum(value);
}

// Threat and other bitflag helpers.
inline ThreatType operator|(ThreatType lhs, ThreatType rhs) {
  return legacy_operators::OrEnum(lhs, rhs);
}
inline ThreatType operator&(ThreatType lhs, ThreatType rhs) {
  return legacy_operators::AndEnum(lhs, rhs);
}
inline ThreatType operator~(ThreatType value) {
  return legacy_operators::NotEnum(value);
}

// Postfix increment helpers for the various enums declared in defines.h.
inline ThemeType operator++(ThemeType& value, int) { return legacy_operators::PostIncrementEnum(value); }
inline ScenarioPlayerType operator++(ScenarioPlayerType& value, int) {
  return legacy_operators::PostIncrementEnum(value);
}
inline ScenarioDirType operator++(ScenarioDirType& value, int) { return legacy_operators::PostIncrementEnum(value); }
inline ScenarioVarType operator++(ScenarioVarType& value, int) { return legacy_operators::PostIncrementEnum(value); }
inline LayerType operator++(LayerType& value, int) { return legacy_operators::PostIncrementEnum(value); }
inline BulletType operator++(BulletType& value, int) { return legacy_operators::PostIncrementEnum(value); }
inline StructType operator++(StructType& value, int) { return legacy_operators::PostIncrementEnum(value); }
inline OverlayType operator++(OverlayType& value, int) { return legacy_operators::PostIncrementEnum(value); }
inline InfantryType operator++(InfantryType& value, int) { return legacy_operators::PostIncrementEnum(value); }
inline UnitType operator++(UnitType& value, int) { return legacy_operators::PostIncrementEnum(value); }
inline AircraftType operator++(AircraftType& value, int) { return legacy_operators::PostIncrementEnum(value); }
inline TemplateType operator++(TemplateType& value, int) { return legacy_operators::PostIncrementEnum(value); }
inline TerrainType operator++(TerrainType& value, int) { return legacy_operators::PostIncrementEnum(value); }
inline SmudgeType operator++(SmudgeType& value, int) { return legacy_operators::PostIncrementEnum(value); }
inline AnimType operator++(AnimType& value, int) { return legacy_operators::PostIncrementEnum(value); }
inline DoType operator++(DoType& value, int) { return legacy_operators::PostIncrementEnum(value); }
inline TheaterType operator++(TheaterType& value, int) { return legacy_operators::PostIncrementEnum(value); }
inline BSizeType operator++(BSizeType& value, int) { return legacy_operators::PostIncrementEnum(value); }
inline FacingType operator++(FacingType& value, int) { return legacy_operators::PostIncrementEnum(value); }
