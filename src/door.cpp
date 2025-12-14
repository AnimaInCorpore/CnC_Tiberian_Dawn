/*
**	Command & Conquer(tm) — Door port
*/

#include "function.h"
#include "door.h"

DoorClass::DoorClass(void)
{
    State = IS_CLOSED;
    IsToRedraw = false;
    Stages = 0;
}

void DoorClass::AI(void)
{
    if (Control.Graphic_Logic()) {
        if (Control.Fetch_Stage() >= Stages) {
            Control.Set_Rate(0);
            switch (State) {
                case IS_OPENING:
                    State = IS_OPEN;
                    break;

                case IS_CLOSING:
                    State = IS_CLOSED;
                    break;
            }
        }
        IsToRedraw = true;
    }
}

bool DoorClass::Open_Door(int rate, int stages)
{
    switch (State) {
        case IS_CLOSED:
        case IS_CLOSING:
            State = IS_OPENING;
            Stages = stages - 1;
            Control.Set_Stage(0);
            Control.Set_Rate(rate);
            return true;
    }
    return false;
}

bool DoorClass::Close_Door(int rate, int stages)
{
    switch (State) {
        case IS_OPEN:
        case IS_OPENING:
            State = IS_CLOSING;
            Stages = stages - 1;
            Control.Set_Stage(0);
            Control.Set_Rate(rate);
            return true;
    }
    return false;
}

int DoorClass::Door_Stage(void) const
{
    switch (State) {
        case IS_CLOSING:
            return ((Stages - 1) - Control.Fetch_Stage());

        case IS_CLOSED:
            return 0;

        case IS_OPENING:
            return Control.Fetch_Stage();

        case IS_OPEN:
            return (Stages - 1);
    }
    return 0;
}
