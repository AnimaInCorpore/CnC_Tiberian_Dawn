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

/* $Header:   F:\\projects\\c&c\\vcs\\code\\slider.cpv   2.17   16 Oct 1995 16:51:54   JOE_BOSTIC  $ */

#include "slider.h"

#include "function.h"

SliderClass::SliderClass(unsigned id, int x, int y, int w, int h, int belong_to_list) : GaugeClass(id, x, y, w, h)
{
    BelongToList = (belong_to_list != 0);

    PlusGadget = 0;
    MinusGadget = 0;

    if (!BelongToList) {
        PlusGadget = new ShapeButtonClass(id, MixFileClass::Retrieve("BTN-PLUS.SHP"), X + Width + 2, Y);
        MinusGadget = new ShapeButtonClass(id, MixFileClass::Retrieve("BTN-MINS.SHP"), X - 6, Y);

        if (PlusGadget) {
            PlusGadget->Make_Peer(*this);
            Add_Tail(*PlusGadget);
            PlusGadget->Flag_To_Redraw();
        }
        if (MinusGadget) {
            MinusGadget->Make_Peer(*this);
            Add_Tail(*MinusGadget);
            MinusGadget->Flag_To_Redraw();
        }
    }

    Set_Thumb_Size(1);
    Recalc_Thumb();

    IsColorized = 0;
}

SliderClass::~SliderClass(void)
{
    if (PlusGadget) {
        PlusGadget->Remove();
        delete PlusGadget;
        PlusGadget = 0;
    }
    if (MinusGadget) {
        MinusGadget->Remove();
        delete MinusGadget;
        MinusGadget = 0;
    }
}

int SliderClass::Set_Maximum(int value)
{
    if (GaugeClass::Set_Maximum(value)) {
        Recalc_Thumb();
        return true;
    }
    return false;
}

void SliderClass::Set_Thumb_Size(int value)
{
    Thumb = MIN(value, MaxValue);
    Thumb = MAX(Thumb, 1);
    Flag_To_Redraw();
    Recalc_Thumb();
}

int SliderClass::Set_Value(int value)
{
    value = MIN(value, MaxValue - Thumb);

    if (GaugeClass::Set_Value(value)) {
        Recalc_Thumb();
        return true;
    }
    return false;
}

void SliderClass::Recalc_Thumb(void)
{
    int length = IsHorizontal ? Width : Height;
    int size = Fixed_To_Cardinal(length, Cardinal_To_Fixed(MaxValue, Thumb));
    ThumbSize = MAX(size, 4);
    int start = Fixed_To_Cardinal(length, Cardinal_To_Fixed(MaxValue, CurValue));
    ThumbStart = MIN(start, length - ThumbSize);
}

int SliderClass::Action(unsigned flags, KeyNumType& key)
{
    if (flags & LEFTPRESS) {
        int mouse;
        int edge;

        if (IsHorizontal) {
            mouse = Get_Mouse_X();
            edge = X;
        } else {
            mouse = Get_Mouse_Y();
            edge = Y;
        }
        edge += 1;

        if (mouse < edge + ThumbStart) {
            Bump(true);
            GaugeClass::Action(0, key);
            key = KN_NONE;
            return true;
        } else if (mouse > edge + ThumbStart + ThumbSize) {
            Bump(false);
            GaugeClass::Action(0, key);
            key = KN_NONE;
            return true;
        } else {
            GaugeClass::Action(flags, key);
            key = KN_NONE;
            return true;
        }
    }

    return GaugeClass::Action(flags, key);
}

int SliderClass::Bump(int up)
{
    if (up) {
        return Set_Value(CurValue - Thumb);
    }
    return Set_Value(CurValue + Thumb);
}

int SliderClass::Step(int up)
{
    if (up) {
        return Set_Value(CurValue - 1);
    }
    return Set_Value(CurValue + 1);
}

void SliderClass::Draw_Thumb(void)
{
    if (IsHorizontal) {
        Draw_Box(X + ThumbStart, Y, ThumbSize, Height, BOXSTYLE_GREEN_RAISED, true);
    } else {
        Draw_Box(X, Y + ThumbStart, Width, ThumbSize, BOXSTYLE_GREEN_RAISED, true);
    }
}

int SliderClass::Draw_Me(int forced)
{
    if (BelongToList) {
        if (ControlClass::Draw_Me(forced)) {
            if (LogicPage == &SeenBuff) {
                Conditional_Hide_Mouse(X, Y, X + Width, Y + Height);
            }

            Draw_Box(X, Y, Width, Height, BOXSTYLE_GREEN_DOWN, true);
            Draw_Thumb();

            if (LogicPage == &SeenBuff) {
                Conditional_Show_Mouse();
            }
            return true;
        }
    }

    return GaugeClass::Draw_Me(forced);
}

void SliderClass::Peer_To_Peer(unsigned flags, KeyNumType&, ControlClass& whom)
{
    if (flags & LEFTRELEASE) {
        if (&whom == PlusGadget) {
            Step(false);
        }
        if (&whom == MinusGadget) {
            Step(true);
        }
    }
}

