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

/* $Header:   F:\\projects\\c&c\\vcs\\code\\dial8.cpv   2.18   16 Oct 1995 16:51:32   JOE_BOSTIC  $ */

#include "dial8.h"

#include "legacy_compat.h"

Dial8Class::Dial8Class(int id, int x, int y, int w, int h, DirType dir) :
    ControlClass(id, x, y, w, h, LEFTPRESS | LEFTHELD | LEFTRELEASE, true)
{
    FaceX = X + (Width / 2);
    FaceY = Y + (Height / 2);

    Direction = dir;
    Facing = Dir_Facing(Direction);
    OldFacing = Facing;

    FacePoint[0][0] = FaceX;
    FacePoint[0][1] = FaceY - (h * 8 / 2) / 10;

    FacePoint[1][0] = FaceX + (w * 7 * 8 / 2) / 100;
    FacePoint[1][1] = FaceY - (h * 7 * 8 / 2) / 100;

    FacePoint[2][0] = FaceX + (w * 8 / 2) / 10;
    FacePoint[2][1] = FaceY;

    FacePoint[3][0] = FaceX + (w * 7 * 8 / 2) / 100;
    FacePoint[3][1] = FaceY + (h * 7 * 8 / 2) / 100;

    FacePoint[4][0] = FaceX;
    FacePoint[4][1] = FaceY + (h * 8 / 2) / 10;

    FacePoint[5][0] = FaceX - (w * 7 * 8 / 2) / 100;
    FacePoint[5][1] = FaceY + (h * 7 * 8 / 2) / 100;

    FacePoint[6][0] = FaceX - (w * 8 / 2) / 10;
    FacePoint[6][1] = FaceY;

    FacePoint[7][0] = FaceX - (w * 7 * 8 / 2) / 100;
    FacePoint[7][1] = FaceY - (h * 7 * 8 / 2) / 100;

    FaceLine[0][0] = FaceX;
    FaceLine[0][1] = FaceY - (h * 6 / 2) / 10;

    FaceLine[1][0] = FaceX + (w * 7 * 6 / 2) / 100;
    FaceLine[1][1] = FaceY - (h * 7 * 6 / 2) / 100;

    FaceLine[2][0] = FaceX + (w * 6 / 2) / 10;
    FaceLine[2][1] = FaceY;

    FaceLine[3][0] = FaceX + (w * 7 * 6 / 2) / 100;
    FaceLine[3][1] = FaceY + (h * 7 * 6 / 2) / 100;

    FaceLine[4][0] = FaceX;
    FaceLine[4][1] = FaceY + (h * 6 / 2) / 10;

    FaceLine[5][0] = FaceX - (w * 7 * 6 / 2) / 100;
    FaceLine[5][1] = FaceY + (h * 7 * 6 / 2) / 100;

    FaceLine[6][0] = FaceX - (w * 6 / 2) / 10;
    FaceLine[6][1] = FaceY;

    FaceLine[7][0] = FaceX - (w * 7 * 6 / 2) / 100;
    FaceLine[7][1] = FaceY - (h * 7 * 6 / 2) / 100;
}

int Dial8Class::Action(unsigned flags, KeyNumType& key)
{
    static int is_sel = 0;

    Sticky_Process(flags);

    if (flags & LEFTPRESS) {
        is_sel = 1;
    }

    if (flags & LEFTPRESS || ((flags & LEFTHELD) && is_sel)) {
        Direction = (DirType)Desired_Facing8(FaceX, FaceY, Get_Mouse_X(), Get_Mouse_Y());

        Facing = Dir_Facing(Direction);

        if (Facing != OldFacing) {
            OldFacing = Facing;
            ControlClass::Action(flags, key);
            return true;
        }

        key = KN_NONE;
        ControlClass::Action(0, key);
        return true;
    }

    if (flags & LEFTRELEASE) {
        key = KN_NONE;
        is_sel = 0;
    }
    return ControlClass::Action(0, key);
}

int Dial8Class::Draw_Me(int forced)
{
    if (ControlClass::Draw_Me(forced)) {
        if (LogicPage == &SeenBuff) {
            Hide_Mouse();
        }

        Draw_Box(X, Y, Width, Height, BOXSTYLE_GREEN_DOWN, true);
        for (int i = 0; i < 8; i++) {
            Draw_Box(FacePoint[i][0] - 1, FacePoint[i][1] - 1, 3, 3, BOXSTYLE_GREEN_RAISED, false);
        }

        LogicPage->Draw_Line(FaceX + 1,
                             FaceY + 1,
                             FaceLine[Facing][0] + 1,
                             FaceLine[Facing][1] + 1,
                             CC_GREEN_SHADOW);
        LogicPage->Draw_Line(FaceX, FaceY, FaceLine[Facing][0], FaceLine[Facing][1], CC_LIGHT_GREEN);

        if (LogicPage == &SeenBuff) {
            Show_Mouse();
        }

        return true;
    }

    return false;
}

DirType Dial8Class::Get_Direction(void) const { return Direction; }

void Dial8Class::Set_Direction(DirType dir)
{
    Direction = dir;
    Facing = Dir_Facing(Direction);
    OldFacing = Facing;
    Flag_To_Redraw();
}
