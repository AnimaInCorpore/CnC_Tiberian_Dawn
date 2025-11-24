#ifndef TARGET_H
#define TARGET_H

#include <cstdint>
#include "defines.h"

/*
** When a unit proceeds with carrying out its mission, it can have several
** intermediate goals. Each goal (or target if you will) can be one of the
** following kinds.
*/
enum KindType {
	KIND_NONE,
	KIND_CELL,
	KIND_UNIT,
	KIND_INFANTRY,
	KIND_BUILDING,
	KIND_TERRAIN,
	KIND_AIRCRAFT,
	KIND_TEMPLATE,
	KIND_BULLET,
	KIND_ANIMATION,
	KIND_TRIGGER,
	KIND_TEAM,
	KIND_TEAMTYPE
};

constexpr unsigned int TARGET_MANTISSA = 12; // Bits of value precision.
constexpr unsigned int TARGET_MANTISSA_MASK = (~((~0u) << TARGET_MANTISSA));
constexpr unsigned int TARGET_EXPONENT = (sizeof(TARGET) * 8) - TARGET_MANTISSA;
constexpr unsigned int TARGET_EXPONENT_MASK = (~((~0u) >> TARGET_EXPONENT));

inline KindType Target_Kind(TARGET a) { return static_cast<KindType>(static_cast<unsigned>(a) >> TARGET_MANTISSA); }
inline unsigned Target_Value(TARGET a) { return (static_cast<unsigned>(a) & TARGET_MANTISSA_MASK); }

inline bool Is_Target_Team(TARGET a) { return (Target_Kind(a) == KIND_TEAM); }
inline bool Is_Target_TeamType(TARGET a) { return (Target_Kind(a) == KIND_TEAMTYPE); }
inline bool Is_Target_Trigger(TARGET a) { return (Target_Kind(a) == KIND_TRIGGER); }
inline bool Is_Target_Infantry(TARGET a) { return (Target_Kind(a) == KIND_INFANTRY); }
inline bool Is_Target_Bullet(TARGET a) { return (Target_Kind(a) == KIND_BULLET); }
inline bool Is_Target_Terrain(TARGET a) { return (Target_Kind(a) == KIND_TERRAIN); }
inline bool Is_Target_Cell(TARGET a) { return (Target_Kind(a) == KIND_CELL); }
inline bool Is_Target_Unit(TARGET a) { return (Target_Kind(a) == KIND_UNIT); }
inline bool Is_Target_Building(TARGET a) { return (Target_Kind(a) == KIND_BUILDING); }
inline bool Is_Target_Template(TARGET a) { return (Target_Kind(a) == KIND_TEMPLATE); }
inline bool Is_Target_Aircraft(TARGET a) { return (Target_Kind(a) == KIND_AIRCRAFT); }
inline bool Is_Target_Animation(TARGET a) { return (Target_Kind(a) == KIND_ANIMATION); }

inline TARGET Build_Target(KindType kind, int value) {
	return static_cast<TARGET>((static_cast<unsigned>(kind) << TARGET_MANTISSA) | static_cast<unsigned>(value));
}
inline TARGET As_Target(CELL cell) { return static_cast<TARGET>((static_cast<unsigned>(KIND_CELL) << TARGET_MANTISSA) | cell); }

class UnitClass;
class BuildingClass;
class TechnoClass;
class TerrainClass;
class ObjectClass;
class InfantryClass;
class BulletClass;
class TriggerClass;
class TeamClass;
class TeamTypeClass;
class AnimClass;
class AircraftClass;

#endif
