#pragma once

#include "legacy_compat.h"

/*
 * Portable build stub for legacy INFANTRY.H.
 *
 * The original infantry runtime depends on the full type system (TYPE.H),
 * animation/action tables (IDATA.CPP), radio/missions, and the complete
 * Techno/Object/Map stacks. Those are not yet ported in the SDL 1.2 build.
 *
 * Keep this header minimal: it provides the class name and a small surface
 * area so other modules can include `infantry.h` without pulling in the full
 * legacy dependency graph.
 */

class InfantryClass : public FootClass {
public:
    InfantryClass();
    virtual ~InfantryClass();

    virtual RTTIType What_Am_I(void) const;
    virtual bool Is_Infantry(void) const { return true; }
    virtual TARGET As_Target(void) const;
};

