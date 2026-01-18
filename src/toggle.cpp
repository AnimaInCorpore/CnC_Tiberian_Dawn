/*
**  Command & Conquer(tm)
**  Copyright 2025 Electronic Arts Inc.
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* $Header:   F:\\projects\\c&c\\vcs\\code\\toggle.cpv   2.18   16 Oct 1995 16:50:56   JOE_BOSTIC  $ */

#include "toggle.h"

#include "function.h"

ToggleClass::ToggleClass(unsigned id, int x, int y, int w, int h)
    : ControlClass(id, x, y, w, h, LEFTPRESS | LEFTRELEASE, true)
{
    IsPressed = false;
    IsOn = false;
    IsToggleType = false;
}

void ToggleClass::Turn_On(void)
{
    IsOn = true;
    Flag_To_Redraw();
}

void ToggleClass::Turn_Off(void)
{
    IsOn = false;
    Flag_To_Redraw();
}

int ToggleClass::Action(unsigned flags, KeyNumType& key)
{
    /*
    ** If there are no action flag bits set, then this must be a forced call.
    ** A forced call must never actually function like a real call, but rather
    ** only performs any necessary graphic updating.
    */
    if (!flags) {
        if ((unsigned)(Get_Mouse_X() - X) < (unsigned)Width && (unsigned)(Get_Mouse_Y() - Y) < (unsigned)Height) {
            if (!IsPressed) {
                IsPressed = true;
                Flag_To_Redraw();
            }
        } else {
            if (IsPressed) {
                IsPressed = false;
                Flag_To_Redraw();
            }
        }
    }

    /*
    ** Handle sticky state for this gadget. It must be processed here because
    ** event flags might be cleared before calling the base class Action().
    */
    Sticky_Process(flags);

    /*
    ** Flag the button to show pressed imagery if mouse was pressed over it.
    */
    if (flags & LEFTPRESS) {
        IsPressed = true;
        Flag_To_Redraw();
        flags &= ~LEFTPRESS;
        ControlClass::Action(flags, key);
        key = KN_NONE;
        return true;
    }

    if (flags & LEFTRELEASE) {
        if (IsPressed) {
            if (IsToggleType) {
                IsOn = (IsOn == false);
            }
            IsPressed = false;
        } else {
            flags &= ~LEFTRELEASE;
        }
    }

    return ControlClass::Action(flags, key);
}

