/*
**	Command & Conquer(tm)
**	Ported combat helpers (Modify_Damage, Explosion_Damage)
*/

#include "legacy/function.h"

int Modify_Damage(int damage, WarheadType warhead, ArmorType armor, int distance)
{
    if (Special.IsInert || !damage || warhead == WARHEAD_NONE) return 0;

    WarheadTypeClass const * whead = &Warheads[warhead];

    damage = Fixed_To_Cardinal(damage, whead->Modifier[armor]);

    if (damage) {
        distance >>= whead->SpreadFactor;
        distance = Bound(distance, 0, 16);
        damage >>= distance;
    }

    return damage;
}

void Explosion_Damage(COORDINATE coord, unsigned strength, TechnoClass * source, WarheadType warhead)
{
    CELL cell;
    ObjectClass *object;
    ObjectClass *objects[32];
    int distance;
    int range;
    int index;
    int count;

    if (!strength || Special.IsInert || warhead == WARHEAD_NONE) return;

    WarheadTypeClass const * whead = &Warheads[warhead];
    range = ICON_LEPTON_W + (ICON_LEPTON_W >> 1);
    cell = Coord_Cell(coord);
    if ((unsigned)cell >= MAP_CELL_TOTAL) return;

    CellClass * cellptr = &Map[cell];
    ObjectClass * impacto = cellptr->Cell_Occupier();

    count = 0;
    for (FacingType i = FACING_NONE; i < FACING_COUNT; i++) {
        if (i != FACING_NONE) {
            CELL adj = Adjacent_Cell(cell, i);
            cellptr = &Map[adj];
        }

        object = cellptr->Cell_Occupier();
        while (object) {
            if (!object->IsToDamage && object != source) {
                object->IsToDamage = true;
                objects[count++] = object;
                if (count >= (int)(sizeof(objects)/sizeof(objects[0]))) break;
            }
            object = object->Next;
        }
        if (count >= (int)(sizeof(objects)/sizeof(objects[0]))) break;
    }

    for (index = 0; index < count; index++) {
        object = objects[index];

        object->IsToDamage = false;
        if (object->What_Am_I() == RTTI_BUILDING && impacto == object) {
            distance = 0;
        } else {
            distance = Distance(coord, object->Center_Coord());
        }
        if (object->IsDown && !object->IsInLimbo && distance < range) {
            int damage = strength;

            if (warhead == WARHEAD_HE && object->What_Am_I() == RTTI_AIRCRAFT) {
                damage *= 2;
            }

            if (damage) {
                object->Take_Damage(damage, distance, warhead, source);
            }
        }
    }

    cellptr = &Map[cell];
    cellptr->Reduce_Tiberium(strength / 10);
    if (cellptr->Overlay != OVERLAY_NONE) {
        OverlayTypeClass const * optr = &OverlayTypeClass::As_Reference(cellptr->Overlay);

        if (optr->IsWall) {
            if (whead->IsWallDestroyer || (whead->IsWoodDestroyer && optr->IsWooden)) {
                Map[cell].Reduce_Wall(strength);
            }
        }
    }
}
