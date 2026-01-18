/*
**  Command & Conquer(tm)
**
**  Portable build note:
**  The original `FINDPATH.CPP` implements the full LOS + edge-following
**  pathing logic and depends on large parts of the movement and map stack
**  (FootClass state machine, passability/threat rules, debug drawing).
**
**  For now we provide a minimal, deterministic, compile-time compatible
**  implementation that builds a simple straight-line move list using the
**  existing `Can_Enter_Cell` shim. This keeps call sites linkable while the
**  full subsystem is ported.
*/

#include "function.h"

#include "path.h"

static int Build_Straight_Line_Path(FootClass const* mover,
                                    CELL start,
                                    CELL dest,
                                    FacingType* moves,
                                    int maxlen,
                                    MoveType threshold)
{
    if (!moves || maxlen <= 0) return 0;

    CELL current = start;
    int length = 0;

    while (current != dest && length < (maxlen - 1)) {
        FacingType facing = Dir_Facing(::Direction(Cell_Coord(current), Cell_Coord(dest)));
        if (facing < FACING_FIRST || facing >= FACING_COUNT) break;

        CELL next = static_cast<CELL>(current + AdjacentCell[facing]);
        if ((unsigned)next >= (unsigned)MAP_CELL_TOTAL) break;

        if (mover) {
            const MoveType allowed = mover->Can_Enter_Cell(next, facing);
            if (allowed > threshold) break;
        }

        moves[length++] = facing;
        current = next;
    }

    moves[length] = FACING_NONE;
    return length;
}

PathType* FootClass::Find_Path(CELL dest, FacingType* final_moves, int maxlen, MoveType threshhold)
{
    static PathType path;

    const CELL start = Coord_Cell(Center_Coord());
    const int length = Build_Straight_Line_Path(this, start, dest, final_moves, maxlen, threshhold);

    path.Start = start;
    path.Cost = 0;
    path.Length = length;
    path.Command = final_moves;
    path.Overlap = NULL;
    path.LastOverlap = -1;
    path.LastFixup = -1;

    return &path;
}

int FootClass::Optimize_Moves(PathType* path, MoveType)
{
    if (!path) return 0;
    return path->Length;
}

bool FootClass::Follow_Edge(CELL,
                            CELL,
                            PathType*,
                            FacingType,
                            FacingType,
                            int,
                            int,
                            int,
                            MoveType)
{
    return false;
}
