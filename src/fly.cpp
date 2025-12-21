#include <cstdlib>

#include "legacy/function.h"

ImpactType FlyClass::Physics(COORDINATE& coord, DirType facing)
{
    if (SpeedAdd != MPH_IMMOBILE) {
        int actual = static_cast<int>(SpeedAdd) + static_cast<int>(SpeedAccum);
        div_t result = div(actual, PIXEL_LEPTON_W);
        SpeedAccum = static_cast<unsigned>(result.rem);
        actual -= result.rem;
        COORDINATE old = coord;

        if (result.quot) {
            COORDINATE newcoord;

            newcoord = Coord_Move(coord, facing, actual);

            if (newcoord == coord) {
                return (IMPACT_NONE);
            }

            coord = newcoord;

            if (newcoord & 0xC000C000L /*|| !Map.In_Radar(Coord_Cell(newcoord))*/) {
                coord = old;
                return (IMPACT_EDGE);
            }

            return (IMPACT_NORMAL);
        }
    }
    return (IMPACT_NONE);
}

void FlyClass::Fly_Speed(int speed, MPHType maximum)
{
    SpeedAdd = static_cast<MPHType>(Fixed_To_Cardinal(static_cast<int>(maximum), speed));
}

