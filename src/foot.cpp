/*
**  Command & Conquer(tm)
**
**  Portable build note:
**  The original `FOOT.CPP` implements the shared "ground unit" brain
**  (movement, mission handlers, map interaction, and UI click handling).
**
**  In the portable build, `FootClass` is still a placeholder living in
**  `src/legacy_compat.h` while core dependencies (e.g., `TechnoClass`,
**  `ObjectClass`, `Map`, `Team`, `Mission`, and infantry/unit special cases)
**  are brought up module-by-module.
**
**  Keep a stub translation unit here so the CMake + SDL 1.2 build remains
**  stable and the filename is reserved for the eventual full port.
*/

#include "function.h"

