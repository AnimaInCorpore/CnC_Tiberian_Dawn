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

/* $Header:   F:\\projects\\c&c\\vcs\\code\\gauge.cpv   2.19   16 Oct 1995 16:50:56   JOE_BOSTIC  $ */

#include "gauge.h"

#include "function.h"

GaugeClass::GaugeClass(unsigned id, int x, int y, int w, int h) :
    ControlClass(id, x, y, w, h, LEFTHELD | LEFTPRESS | LEFTRELEASE, true)
{
    Set_Maximum(255);
    Set_Value(0);

    HasThumb = true;
    IsHorizontal = (w > h);
    IsColorized = true;

    ClickDiff = 0;
}

int GaugeClass::Set_Maximum(int value)
{
    if (value != MaxValue) {
        MaxValue = value;
        Flag_To_Redraw();
        return (true);
    }
    return (false);
}

int GaugeClass::Set_Value(int value)
{
    value = Bound(value, 0, MaxValue);
    if (value != CurValue) {
        CurValue = value;
        Flag_To_Redraw();
        return (true);
    }
    return (false);
}

int GaugeClass::Pixel_To_Value(int pixel)
{
    int maximum;

    if (IsHorizontal) {
        pixel -= X + 1;
        maximum = Width;
    } else {
        pixel -= Y + 1;
        maximum = Height;
    }
    maximum -= 2;
    pixel = Bound(pixel, 0, maximum);
    return (Fixed_To_Cardinal(MaxValue, Cardinal_To_Fixed(maximum, pixel)));
}

int GaugeClass::Value_To_Pixel(int value)
{
    int maximum;
    int start;
    if (IsHorizontal) {
        maximum = Width;
        start = X;
    } else {
        maximum = Height;
        start = Y;
    }
    maximum -= 2;
    return (start + Fixed_To_Cardinal(maximum, Cardinal_To_Fixed(MaxValue, value)));
}

int GaugeClass::Draw_Me(int forced)
{
    if (ControlClass::Draw_Me(forced)) {
        if (LogicPage == &HidPage || LogicPage == &SeenBuff) {
            Conditional_Hide_Mouse(X, Y, X + Width, Y + Height);
        }

        Draw_Box(X, Y, Width, Height, BOXSTYLE_GREEN_DOWN, true);

        if (IsColorized) {
            int middle = Value_To_Pixel(CurValue);
            int color = CC_BRIGHT_GREEN;
            if (IsHorizontal) {
                if (middle >= (X + 1)) LogicPage->Fill_Rect(X + 1, Y + 1, middle, Y + Height - 2, color);
            } else {
                if (middle >= (Y + 1)) LogicPage->Fill_Rect(X + 1, Y + 1, X + Width - 2, middle, color);
            }
        }

        if (HasThumb) Draw_Thumb();

        if (LogicPage == &HidPage || LogicPage == &SeenBuff) {
            Conditional_Show_Mouse();
        }
        return (true);
    }
    return (false);
}

int GaugeClass::Action(unsigned flags, KeyNumType& key)
{
    if (!HasThumb) {
        key = KN_NONE;
        return (true);
    }

    Sticky_Process(flags);

    if ((flags & LEFTPRESS) || ((flags & LEFTHELD) && StuckOn == this)) {
        if (flags & LEFTPRESS) {
            int curpix = Value_To_Pixel(CurValue);
            int clickpix = (IsHorizontal ? Get_Mouse_X() : Get_Mouse_Y());

            if ((clickpix > curpix) && (clickpix - curpix) < Thumb_Pixels()) {
                ClickDiff = (clickpix - curpix);
            } else {
                ClickDiff = 0;
            }

            int testval = Pixel_To_Value(IsHorizontal ? Get_Mouse_X() - ClickDiff : Get_Mouse_Y() - ClickDiff);

            while (testval < CurValue && ClickDiff > 0) {
                ClickDiff--;
                testval = Pixel_To_Value(IsHorizontal ? Get_Mouse_X() - ClickDiff : Get_Mouse_Y() - ClickDiff);
            }
        }

        if (!Set_Value(Pixel_To_Value(IsHorizontal ? Get_Mouse_X() - ClickDiff : Get_Mouse_Y() - ClickDiff))) {
            flags &= ~(LEFTHELD | LEFTRELEASE | LEFTPRESS);
            ControlClass::Action(0, key);
            key = KN_NONE;
            return (true);
        }

    } else {
        flags &= ~LEFTHELD;
    }
    return (ControlClass::Action(flags, key));
}

void GaugeClass::Draw_Thumb(void)
{
    int x = Value_To_Pixel(CurValue);

    if ((x + 4) > Value_To_Pixel(MaxValue)) {
        x = Value_To_Pixel(MaxValue) - 2;
    }

    if (IsHorizontal) {
        Draw_Box(x, Y, 4, Height, BOXSTYLE_GREEN_RAISED, true);
    } else {
        Draw_Box(X, x, Width, 4, BOXSTYLE_GREEN_RAISED, true);
    }
}

TriColorGaugeClass::TriColorGaugeClass(unsigned id, int x, int y, int w, int h) : GaugeClass(id, x, y, w, h)
{
    RedLimit = 0;
    YellowLimit = 0;
}

int TriColorGaugeClass::Set_Red_Limit(int value)
{
    if (value >= 0 && value < MaxValue) {
        RedLimit = value;
        Flag_To_Redraw();
        return (true);
    }
    return (false);
}

int TriColorGaugeClass::Set_Yellow_Limit(int value)
{
    if (value >= 0 && value < MaxValue) {
        YellowLimit = value;
        Flag_To_Redraw();
        return (true);
    }
    return (false);
}

int TriColorGaugeClass::Draw_Me(int forced)
{
    if (ControlClass::Draw_Me(forced)) {
        if (LogicPage == &HidPage || LogicPage == &SeenBuff) {
            Conditional_Hide_Mouse(X, Y, X + Width, Y + Height);
        }

        Draw_Box(X, Y, Width, Height, (IsDisabled ? BOXSTYLE_GREEN_RAISED : BOXSTYLE_GREEN_DOWN), true);

        int red = Value_To_Pixel(RedLimit);
        int yellow = Value_To_Pixel(YellowLimit);
        int middle = Value_To_Pixel(CurValue);

        if (CurValue <= RedLimit) {
            if (IsHorizontal) {
                LogicPage->Fill_Rect(X + 1, Y + 1, middle, Y + Height - 2, PINK);
            } else {
                LogicPage->Fill_Rect(X + 1, Y + 1, X + Width - 2, middle, PINK);
            }
        } else if (CurValue > RedLimit && CurValue <= YellowLimit) {
            if (IsHorizontal) {
                LogicPage->Fill_Rect(X + 1, Y + 1, red, Y + Height - 2, PINK);
                LogicPage->Fill_Rect(red, Y + 1, middle, Y + Height - 2, YELLOW);
            } else {
                LogicPage->Fill_Rect(X + 1, Y + 1, X + Width - 2, red, PINK);
                LogicPage->Fill_Rect(X + 1, red, X + Width - 2, middle, YELLOW);
            }
        } else if (CurValue > YellowLimit && CurValue <= MaxValue) {
            if (IsHorizontal) {
                LogicPage->Fill_Rect(X + 1, Y + 1, red, Y + Height - 2, PINK);
                LogicPage->Fill_Rect(red, Y + 1, yellow, Y + Height - 2, YELLOW);
                LogicPage->Fill_Rect(yellow, Y + 1, middle, Y + Height - 2, GREEN);
            } else {
                LogicPage->Fill_Rect(X + 1, Y + 1, X + Width - 2, red, PINK);
                LogicPage->Fill_Rect(X + 1, red, X + Width - 2, yellow, YELLOW);
                LogicPage->Fill_Rect(X + 1, yellow, X + Width - 2, middle, GREEN);
            }
        }

        if (HasThumb) Draw_Thumb();

        if (LogicPage == &HidPage || LogicPage == &SeenBuff) {
            Conditional_Show_Mouse();
        }
        return (true);
    }
    return (false);
}
