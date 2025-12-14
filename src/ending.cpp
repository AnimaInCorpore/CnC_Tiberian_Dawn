/*
**	Command & Conquer(tm)
**	Copyright 2025 Electronic Arts Inc.
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "legacy/function.h"
#include "legacy/textblit.h"

void GDI_Ending(void)
{
#ifdef DEMO
    Fade_Palette_To(BlackPalette, FADE_PALETTE_MEDIUM, Call_Back);
    Load_Title_Screen("DEMOPIC.PCX", &HidPage, Palette);
    HidPage.Blit(SeenBuff);
    Fade_Palette_To(Palette, FADE_PALETTE_MEDIUM, Call_Back);
    Clear_KeyBuffer();
    Get_Key_Num();
    Fade_Palette_To(BlackPalette, FADE_PALETTE_MEDIUM, Call_Back);
    VisiblePage.Clear();

#else
    if (TempleIoned) {
        Play_Movie("GDIFINB");
    } else {
        Play_Movie("GDIFINA");
    }

    Score.Presentation();

    if (TempleIoned) {
        Play_Movie("GDIEND2");
    } else {
        Play_Movie("GDIEND1");
    }

    // Simplified: trailer/attract screen handling is deferred in the port.
    Play_Movie("CC2TEASE");
#endif
}

#ifndef DEMO
void Nod_Ending(void)
{
    static unsigned char const _tanpal[]={0x0,0xED,0xED,0x2C,0x2C,0xFB,0xFB,0xFD,0xFD,0x0,0x0,0x0,0x0,0x0,0x52,0x0};

    // Simplified stub: display the final movie and present score.
    Play_Movie("NODFINAL");
    Score.Presentation();
    Play_Movie("CC2TEASE");
}

#endif
