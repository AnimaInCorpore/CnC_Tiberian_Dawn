/*
**  Command & Conquer(tm)
**
**  Portable build note:
**  The original `FINDPATH.CPP` is tightly coupled to the full `FootClass`
**  movement state, map/cell passability rules, and debug drawing surfaces
**  (e.g., `SeenBuff`, `Map.Coord_To_Pixel`, and `Debug_Find_Path`).
**
**  Keep a stub translation unit here so the CMake + SDL 1.2 build remains
**  stable while the movement/pathfinding stack is ported.
*/

#include "function.h"

