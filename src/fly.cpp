#include "fly.h"

#include "display.h"

#include <cstdlib>

FlyClass::FlyClass(void) : SpeedAccum(0), SpeedAdd(MPH_IMMOBILE) {}

ImpactType FlyClass::Physics(COORDINATE& coord, DirType facing) {
    if (SpeedAdd != MPH_IMMOBILE) {
        int actual = static_cast<int>(SpeedAdd) + static_cast<int>(SpeedAccum);
        std::div_t result = std::div(actual, PIXEL_LEPTON_W);
        SpeedAccum = static_cast<unsigned>(result.rem);
        actual -= result.rem;
        COORDINATE old = coord;

        /*
        **  If movement occurred that is at least one
        **  pixel, then check update the coordinate and
        **  check for edge of world collision.
        */
        if (result.quot) {
            COORDINATE newcoord; // New working coordinate.

            newcoord = Coord_Move(coord, facing, static_cast<unsigned short>(actual));

            /*
            **  If no movement occurred, then presume it hasn't moved at all
            **  and return immediately with this indication.
            */
            if (newcoord == coord) {
                return IMPACT_NONE;
            }

            /*
            **  Remember the new position.
            */
            coord = newcoord;

            /*
            **  If the new coordinate is off the edge of the world, then report
            **  this.
            */
            if (newcoord & 0xC000C000L /*|| !Map.In_Radar(Coord_Cell(newcoord))*/) {
                coord = old;
                return IMPACT_EDGE;
            }

            return IMPACT_NORMAL;
        }
    }
    return IMPACT_NONE;
}

void FlyClass::Fly_Speed(int speed, MPHType maximum) {
    SpeedAdd = static_cast<MPHType>(Fixed_To_Cardinal(static_cast<int>(maximum), speed));
}

void FlyClass::Code_Pointers(void) {}

void FlyClass::Decode_Pointers(void) {}

