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

/* $Header:   F:\\projects\\c&c\\vcs\\code\\txtlabel.cpv   1.9   16 Oct 1995 16:49:44   JOE_BOSTIC  $ */

#include "txtlabel.h"

#include "function.h"

TextLabelClass::TextLabelClass(char* txt, int x, int y, int color, TextPrintType style)
    : GadgetClass(x, y, 1, 1, 0u, false)
{
    Text = txt;
    Color = color;
    Style = style;
    UserData = 0;
    PixWidth = -1;
    Segments = 0;
    CRC = 0;
}

int TextLabelClass::Draw_Me(int forced)
{
    if (GadgetClass::Draw_Me(forced)) {
        if (PixWidth == -1) {
            Fancy_Text_Print("%s", X, Y, Color, TBLACK, Style, Text);
        } else {
            Conquer_Clip_Text_Print(Text, X, Y, Color, TBLACK, Style, PixWidth);
        }
        return true;
    }
    return false;
}

