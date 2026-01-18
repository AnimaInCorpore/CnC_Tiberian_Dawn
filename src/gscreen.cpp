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

/* $Header:   F:\\projects\\c&c\\vcs\\code\\gscreen.cpv   2.17   16 Oct 1995 16:51:34   JOE_BOSTIC  $ */

#include "gscreen.h"

#include "gadget.h"

GadgetClass* GScreenClass::Buttons = 0;
GraphicBufferClass* GScreenClass::ShadowPage = 0;

GScreenClass::GScreenClass(void) : IsToRedraw(1u), IsToUpdate(1u) {}

void GScreenClass::One_Time(void) {
    Buttons = 0;
    if (!ShadowPage) {
        ShadowPage = new GraphicBufferClass(320, 200);
    }
    if (ShadowPage) {
        ShadowPage->Clear();
    }
}

void GScreenClass::Init(TheaterType theater) {
    Init_Clear();
    Init_IO();
    Init_Theater(theater);
}

void GScreenClass::Init_Clear(void) {
    if (ShadowPage) {
        ShadowPage->Clear();
    }
    IsToRedraw = 1u;
    IsToUpdate = 1u;
}

void GScreenClass::Init_IO(void) { Buttons = 0; }

void GScreenClass::Init_Theater(TheaterType theater) { (void)theater; }

void GScreenClass::Flag_To_Redraw(bool complete) {
    IsToUpdate = 1u;
    if (complete) {
        IsToRedraw = 1u;
    }
}

void GScreenClass::Input(KeyNumType& key, int& x, int& y) {
    key = KN_NONE;
    x = 0;
    y = 0;

    if (Buttons) {
        if (Buttons->Is_List_To_Redraw()) {
            Flag_To_Redraw(false);
        }

        key = Buttons->Input();
    }

    AI(key, x, y);
}

void GScreenClass::Add_A_Button(GadgetClass& gadget) {
    if (Buttons == &gadget) {
        Remove_A_Button(gadget);
    } else {
        gadget.Remove();
    }

    if (Buttons) {
        gadget.Add_Tail(*Buttons);
    } else {
        Buttons = &gadget;
    }
}

void GScreenClass::Remove_A_Button(GadgetClass& gadget) { Buttons = gadget.Remove(); }

void GScreenClass::Render(void) {
    if (!(IsToUpdate || IsToRedraw)) return;

    Draw_It(IsToRedraw ? true : false);

    if (Buttons) {
        Buttons->Draw_All(false);
    }

    Blit_Display();

    IsToUpdate = 0u;
    IsToRedraw = 0u;
}

void GScreenClass::Blit_Display(void)
{
    // Portable build: emulate the C&C95 hidden->seen blit in software.
    HidPage.Blit(SeenBuff);
}

void GScreenClass::Code_Pointers(void) {}

void GScreenClass::Decode_Pointers(void) {}
