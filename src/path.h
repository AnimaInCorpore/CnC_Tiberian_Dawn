#pragma once

#include "legacy_compat.h"

/*
 * Minimal definition extracted from legacy DEFINES.H.
 *
 * Pathfinding (findpath.cpp) and movement code reference this structure.
 * Keep it in a dedicated header rather than growing legacy_compat.h.
 */
struct PathType {
    CELL Start;
    int Cost;
    int Length;
    FacingType* Command;
    unsigned long* Overlap;
    CELL LastOverlap;
    CELL LastFixup;
};

