/*
 * Portable build stub for legacy IDATA.CPP.
 *
 * The original module defines InfantryTypeClass static instances and large
 * animation "DO" tables. Those depend on the full type system (TYPE.H) and
 * infantry runtime (INFANTRY.*), which are not yet ported into `src/`.
 *
 * Keep this translation unit present so the CMake+SDL 1.2 build remains stable
 * while infantry/data modules are migrated.
 */

#include "function.h"

