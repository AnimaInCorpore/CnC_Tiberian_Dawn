#include "legacy/function.h"

namespace {

CELL Clip_Scatter(CELL cell, int maxdist) {
  int x = Cell_X(cell);
  int y = Cell_Y(cell);

  int xmin = Map.MapCellX;
  int xmax = xmin + Map.MapCellWidth - 1;
  int ymin = Map.MapCellY;
  int ymax = ymin + Map.MapCellHeight - 1;

  int xdist = IRandom(0, maxdist);
  if (IRandom(0, 1) == 0) {
    x += xdist;
    if (x > xmax) {
      x = xmax;
    }
  } else {
    x -= xdist;
    if (x < xmin) {
      x = xmin;
    }
  }

  int ydist = IRandom(0, maxdist);
  if (IRandom(0, 1) == 0) {
    y += ydist;
    if (y > ymax) {
      y = ymax;
    }
  } else {
    y -= ydist;
    if (y < ymin) {
      y = ymin;
    }
  }

  return XY_Cell(x, y);
}

CELL Clip_Move(CELL cell, FacingType facing, int dist) {
  int x = Cell_X(cell);
  int y = Cell_Y(cell);

  int xmin = Map.MapCellX;
  int xmax = xmin + Map.MapCellWidth - 1;
  int ymin = Map.MapCellY;
  int ymax = ymin + Map.MapCellHeight - 1;

  switch (facing) {
    case FACING_N:
      y -= dist;
      break;
    case FACING_NE:
      x += dist;
      y -= dist;
      break;
    case FACING_E:
      x += dist;
      break;
    case FACING_SE:
      x += dist;
      y += dist;
      break;
    case FACING_S:
      y += dist;
      break;
    case FACING_SW:
      x -= dist;
      y += dist;
      break;
    case FACING_W:
      x -= dist;
      break;
    case FACING_NW:
      x -= dist;
      y -= dist;
      break;
    default:
      break;
  }

  if (x > xmax) x = xmax;
  if (x < xmin) x = xmin;
  if (y > ymax) y = ymax;
  if (y < ymin) y = ymin;

  return XY_Cell(x, y);
}

}  // namespace

int Scan_Place_Object(ObjectClass* obj, CELL cell) {
  int dist;
  FacingType rot;
  FacingType fcounter;
  int tryval;
  CELL newcell;
  TechnoClass* techno;
  int skipit;

  if (Map.In_Radar(cell)) {
    techno = Map[cell].Cell_Techno();
    if (!techno || (techno->What_Am_I() == RTTI_INFANTRY && obj->What_Am_I() == RTTI_INFANTRY)) {
      if (obj->Unlimbo(Cell_Coord(cell), DIR_N)) {
        return true;
      }
    }
  }

  for (dist = 1; dist < 32; dist++) {
    rot = static_cast<FacingType>(IRandom(FACING_N, FACING_NW));

    for (tryval = 0; tryval < 2; tryval++) {
      for (fcounter = FACING_N; fcounter <= FACING_NW; fcounter++) {
        skipit = false;
        newcell = Clip_Move(cell, rot, dist);

        if (tryval > 0) {
          newcell = Clip_Scatter(newcell, 1);
        }

        if (newcell == cell) {
          skipit = true;
        }

        if (!skipit) {
          techno = Map[newcell].Cell_Techno();
          if (!techno || (techno->What_Am_I() == RTTI_INFANTRY &&
                          obj->What_Am_I() == RTTI_INFANTRY)) {
            if (obj->Unlimbo(Cell_Coord(newcell), DIR_N)) {
              return true;
            }
          }
        }

        rot = static_cast<FacingType>(rot + 1);
        if (rot > FACING_NW) {
          rot = FACING_N;
        }
      }
    }
  }

  return false;
}
