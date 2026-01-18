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

/* $Header:   F:\\projects\\c&c\\vcs\\code\\shapebtn.cpv   2.17   16 Oct 1995 16:52:00   JOE_BOSTIC  $ */

#include "shapebtn.h"

#include "function.h"

ShapeButtonClass::ShapeButtonClass(void) : ToggleClass(0, 0, 0, 0, 0)
{
    ReflectButtonState = false;
    ShapeData = 0;
}

ShapeButtonClass::ShapeButtonClass(unsigned id, void const* shape, int x, int y) : ToggleClass(id, x, y, 0, 0)
{
    Width = 0;
    Height = 0;
    ReflectButtonState = false;
    ShapeData = 0;
    Set_Shape(shape);
}

void ShapeButtonClass::Set_Shape(void const* data)
{
    ShapeData = data;

    /*
    ** SHP decoding isn't ported yet; fall back to a small, consistent button size
    ** so list scroll arrows and +/- buttons have sensible placement.
    */
    if (ShapeData && (Width <= 0 || Height <= 0)) {
        Width = 10;
        Height = 10;
    }
}

int ShapeButtonClass::Draw_Me(int forced)
{
    if (!ControlClass::Draw_Me(forced)) return false;

    if (LogicPage == &SeenBuff) {
        Conditional_Hide_Mouse(X, Y, X + Width - 1, Y + Height - 1);
    }

    const bool down = ReflectButtonState ? (IsOn != 0) : (IsPressed != 0);
    BoxStyleEnum style = BOXSTYLE_GREEN_RAISED;
    if (IsDisabled) {
        style = BOXSTYLE_GREEN_DIS_RAISED;
    } else {
        style = down ? BOXSTYLE_GREEN_DOWN : BOXSTYLE_GREEN_RAISED;
    }

    Draw_Box(X, Y, Width, Height, style, true);

    if (LogicPage == &SeenBuff) {
        Conditional_Show_Mouse();
    }

    return true;
}

