#pragma once

#include "legacy_compat.h"

class AircraftClass : public ObjectClass {
public:
    AircraftClass(AircraftType type = AIRCRAFT_NONE, HousesType house = HOUSE_NONE);
    virtual ~AircraftClass();

    virtual RTTIType What_Am_I(void) const { return RTTI_AIRCRAFT; }

    AircraftType Type;
    HousesType House;
};

