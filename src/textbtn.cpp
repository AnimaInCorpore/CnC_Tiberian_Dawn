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

/* $Header:   F:\\projects\\c&c\\vcs\\code\\textbtn.cpv   2.18   16 Oct 1995 16:49:16   JOE_BOSTIC  $ */

#include "textbtn.h"

#include "function.h"

TextButtonClass::TextButtonClass(void) : ToggleClass(0, 0, 0, 0, 0)
{
    String = 0;
    PrintFlags = (TextPrintType)0;
    IsBlackBorder = false;
}

TextButtonClass::TextButtonClass(unsigned id,
                                 char const* text,
                                 TextPrintType style,
                                 int x,
                                 int y,
                                 int w,
                                 int h,
                                 int blackborder)
    : ToggleClass(id, x, y, 0, 0)
{
    String = text;
    PrintFlags = style;
    IsBlackBorder = (blackborder != 0);

    if (String) {
        Fancy_Text_Print(TXT_NONE, 0, 0, TBLACK, TBLACK, PrintFlags);

        if (w == -1) {
            Width = String_Pixel_Width(String) + 8;
        } else {
            Width = w;
        }

        if (h == -1) {
            Height = FontHeight + FontYSpacing + 2;
        } else {
            Height = h;
        }
    }
}

TextButtonClass::TextButtonClass(unsigned id, int text, TextPrintType style, int x, int y, int w, int h, int blackborder)
    : ToggleClass(id, x, y, 0, 0)
{
    String = 0;
    PrintFlags = style;
    IsBlackBorder = (blackborder != 0);
    Set_Text(text, false);

    if (String) {
        Fancy_Text_Print(TXT_NONE, 0, 0, TBLACK, TBLACK, PrintFlags);

        if (w == -1) {
            Width = String_Pixel_Width(String) + 8;
        } else {
            Width = w;
        }

        if (h == -1) {
            Height = FontHeight + FontYSpacing + 2;
        } else {
            Height = h;
        }
    }
}

int TextButtonClass::Draw_Me(int forced)
{
    if (ControlClass::Draw_Me(forced)) {
        if (LogicPage == &SeenBuff) {
            Conditional_Hide_Mouse(X, Y, X + Width, Y + Height);
        }

        Draw_Background();
        Draw_Text(String);

        if (LogicPage == &SeenBuff) {
            Conditional_Show_Mouse();
        }
        return true;
    }
    return false;
}

void TextButtonClass::Set_Text(char const* text, bool resize)
{
    String = text;
    Flag_To_Redraw();
    if (resize && String) {
        Fancy_Text_Print(TXT_NONE, 0, 0, TBLACK, TBLACK, PrintFlags);
        Width = String_Pixel_Width(String) + 8;
        Height = FontHeight + FontYSpacing + 2;
    }
}

void TextButtonClass::Set_Text(int text, bool resize)
{
    if (text != TXT_NONE) {
        Set_Text(Text_String(text), resize);
    }
}

void TextButtonClass::Draw_Background(void)
{
    if (IsBlackBorder) {
        LogicPage->Draw_Rect(X - 1, Y - 1, X + Width + 2, Y + Height + 2, BLACK);
    }

    BoxStyleEnum style;
    if (PrintFlags & TPF_6PT_GRAD) {
        if (IsDisabled) {
            style = BOXSTYLE_GREEN_DIS_RAISED;
        } else {
            style = IsPressed ? BOXSTYLE_GREEN_DOWN : BOXSTYLE_GREEN_RAISED;
        }
    } else {
        if (IsDisabled) {
            style = BOXSTYLE_DIS_RAISED;
        } else {
            style = IsPressed ? BOXSTYLE_DOWN : BOXSTYLE_RAISED;
        }
    }
    Draw_Box(X, Y, Width, Height, style, true);
}

void TextButtonClass::Draw_Text(char const* text)
{
    if (!text) return;

    if (PrintFlags & TPF_6PT_GRAD) {
        TextPrintType flags;
        const int color = CC_GREEN;

        if (IsDisabled) {
            flags = (TextPrintType)0;
        } else {
            if (IsPressed || IsOn) {
                flags = (TextPrintType)(TPF_USE_GRAD_PAL | TPF_BRIGHT_COLOR);
            } else {
                flags = (TextPrintType)(TPF_USE_GRAD_PAL | TPF_MEDIUM_COLOR);
            }
        }

        Fancy_Text_Print(text, X + (Width >> 1) - 1, Y + 1, color, TBLACK, (TextPrintType)(PrintFlags | flags | TPF_CENTER));
        return;
    }

    int color;
    if (IsDisabled) {
        color = LTGREY;
    } else if (IsPressed) {
        color = (PrintFlags & TPF_NOSHADOW) ? DKGREY : LTGREY;
    } else {
        color = WHITE;
    }

    Fancy_Text_Print(text, X + (Width >> 1) - 1, Y + 1, IsOn ? RED : color, TBLACK, (TextPrintType)(PrintFlags | TPF_CENTER));
}

