#include "legacy/defines.h"
#include "legacy/externs.h"
#include "RAND.h"

#include <cmath>
#include <cstdint>

/*
**  Modern, portable implementations of the core coordinate helpers that were
**  originally split between COORD.CPP and COORDA.ASM.
*/

void Move_Point(short& x, short& y, DirType dir, unsigned short distance);

namespace {

constexpr int ICON_PIXEL_W = 24;
inline int Coord_X(COORDINATE coord) { return static_cast<short>(coord); }
inline int Coord_Y(COORDINATE coord) {
  return static_cast<short>((coord >> 16) & 0xFFFF);
}
inline CELL Coord_XCell(COORDINATE coord) {
  return static_cast<CELL>((coord >> 8) & 0xFF);
}
inline CELL Coord_YCell(COORDINATE coord) {
  return static_cast<CELL>((coord >> 24) & 0xFF);
}
inline COORDINATE XY_Coord(int x, int y) {
  return (static_cast<unsigned>(y) << 16) |
         (static_cast<unsigned>(x) & 0xFFFF);
}
inline COORDINATE Coord_Snap_Helper(COORDINATE coord) {
  return (coord & 0xFF00FF00u) | 0x00800080u;
}

template <class T>
T Random_Pick_Local(T a, T b) {
  return static_cast<T>(IRandom(static_cast<int>(a), static_cast<int>(b)));
}

inline DirType Facing_Dir(FacingType facing) {
  return static_cast<DirType>((static_cast<int>(facing) << 5) & 0xFF);
}
}  // namespace

unsigned Cardinal_To_Fixed(unsigned base, unsigned cardinal) {
  if (base == 0) {
    return 0xFFFF;
  }
  // 8.8 fixed point conversion with simple rounding.
  const unsigned long long scaled =
      (static_cast<unsigned long long>(cardinal) << 8);
  return static_cast<unsigned>((scaled + (base / 2)) / base);
}

unsigned Fixed_To_Cardinal(unsigned base, unsigned fixed) {
  const unsigned long long scaled =
      static_cast<unsigned long long>(base) * fixed + 0x80;
  return static_cast<unsigned>((scaled >> 8) &
                               static_cast<unsigned long long>(0xFFFFFFFF));
}

int Desired_Facing256(int x1, int y1, int x2, int y2) {
  constexpr double kTwoPi = 6.28318530717958647692;
  const int dx = x2 - x1;
  const int dy = y2 - y1;
  if (dx == 0 && dy == 0) {
    return 0;
  }

  const double angle =
      std::atan2(static_cast<double>(dy), static_cast<double>(dx));
  // DirType 64 is due east; shift atan2 output so 0 points north.
  int dir = static_cast<int>(std::lround(angle * (256.0 / kTwoPi))) + 64;
  dir &= 0xFF;
  return dir;
}

int Desired_Facing8(int x1, int y1, int x2, int y2) {
  const int facing256 = Desired_Facing256(x1, y1, x2, y2) & 0xFF;
  const FacingType facing = static_cast<FacingType>(Facing8[facing256]);
  return static_cast<int>(Facing_Dir(facing));
}

short const* Coord_Spillage_List(COORDINATE coord, int maxsize) {
  static short const gigundo[] = {
      -((2 * MAP_CELL_W) - 2), -((2 * MAP_CELL_W) - 1),
      -((2 * MAP_CELL_W)),     -((2 * MAP_CELL_W) + 1),
      -((2 * MAP_CELL_W) + 2), -((1 * MAP_CELL_W) - 2),
      -((1 * MAP_CELL_W) - 1), -((1 * MAP_CELL_W)),
      -((1 * MAP_CELL_W) + 1), -((1 * MAP_CELL_W) + 2),
      -((0 * MAP_CELL_W) - 2), -((0 * MAP_CELL_W) - 1),
      -((0 * MAP_CELL_W)),     -((0 * MAP_CELL_W) + 1),
      -((0 * MAP_CELL_W) + 2), +((1 * MAP_CELL_W) - 2),
      +((1 * MAP_CELL_W) - 1), +((1 * MAP_CELL_W)),
      +((1 * MAP_CELL_W) + 1), +((1 * MAP_CELL_W) + 2),
      +((2 * MAP_CELL_W) - 2), +((2 * MAP_CELL_W) - 1),
      +((2 * MAP_CELL_W)),     +((2 * MAP_CELL_W) + 1),
      +((2 * MAP_CELL_W) + 2), REFRESH_EOL};

  static short const simple_adjacent[] = {
      0,           -MAP_CELL_W, -(MAP_CELL_W - 1), 1, MAP_CELL_W + 1,
      MAP_CELL_W,  MAP_CELL_W - 1, -1,             -(MAP_CELL_W + 1),
      REFRESH_EOL};

  // Very large objects spill across a 5x5 grid.
  if (maxsize > ICON_PIXEL_W * 2) {
    return &gigundo[0];
  }

  // For modestly sized objects, use a conservative adjacent list.
  if (maxsize > ICON_PIXEL_W) {
    return &simple_adjacent[0];
  }

  // Small objects occupy their primary cell only.
  static short const center_only[] = {0, REFRESH_EOL};
  return &center_only[0];
}

COORDINATE Coord_Move(COORDINATE start, DirType dir, unsigned short distance) {
  short x = static_cast<short>(Coord_X(start));
  short y = static_cast<short>(Coord_Y(start));
  Move_Point(x, y, dir, distance);
  return XY_Coord(x, y);
}

COORDINATE Coord_Scatter(COORDINATE coord, unsigned distance, bool lock) {
  COORDINATE newcoord =
      Coord_Move(coord, Random_Pick_Local(DIR_N, DIR_MAX), distance);

  // Reject scatter that would overflow the coordinate packing.
  if (newcoord & 0xC000C000L) {
    newcoord = coord;
  }

  if (lock) {
    newcoord = Coord_Snap_Helper(newcoord);
  }

  return newcoord;
}

CELL Coord_Cell(COORDINATE coord) {
  return static_cast<CELL>(Coord_YCell(coord) * MAP_CELL_W +
                           Coord_XCell(coord));
}

void Move_Point(short& x, short& y, DirType dir, unsigned short distance) {
  constexpr double kTwoPi = 6.28318530717958647692;
  const double radians = (static_cast<int>(dir) - 64) * (kTwoPi / 256.0);
  const double dx = std::cos(radians) * static_cast<double>(distance);
  const double dy = std::sin(radians) * static_cast<double>(distance);

  x = static_cast<short>(std::lround(static_cast<double>(x) + dx));
  y = static_cast<short>(std::lround(static_cast<double>(y) + dy));
}
