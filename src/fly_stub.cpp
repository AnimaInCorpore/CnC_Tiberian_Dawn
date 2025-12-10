#include "legacy/aircraft.h"
#include "legacy/fly.h"
#include "legacy/function.h"

// Lightweight fly/aircraft helpers to satisfy the frontend build.

ImpactType FlyClass::Physics(COORDINATE& coord, DirType facing) {
  // Maintain the current coordinate and report no impact.
  (void)facing;
  return static_cast<ImpactType>(IMPACT_NONE);
}

void FlyClass::Fly_Speed(int /*speed*/, MPHType /*maximum*/) {}

COORDINATE As_Movement_Coord(TARGET target) { return static_cast<COORDINATE>(target); }

void AircraftClass::Code_Pointers(void) {}
void AircraftClass::Decode_Pointers(void) {}
