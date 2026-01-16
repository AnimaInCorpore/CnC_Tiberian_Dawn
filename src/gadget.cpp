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

/* $Header:   F:\\projects\\c&c\\vcs\\code\\gadget.cpv   2.18   16 Oct 1995 16:49:40   JOE_BOSTIC  $ */

#include "gadget.h"

#include "control.h"
#include "function.h"
#include "sdl_platform.h"

GadgetClass* GadgetClass::StuckOn = 0;
GadgetClass* GadgetClass::LastList = 0;
GadgetClass* GadgetClass::Focused = 0;

void GadgetClass::Add_Tail(GadgetClass& gadget) {
    GadgetClass* current = this;
    while (current->Next) {
        current = current->Next;
    }

    gadget.Next = 0;
    gadget.Prev = current;
    current->Next = &gadget;
}

void GadgetClass::Enable(void) {
    IsDisabled = 0u;
    IsToRepaint = 1u;
    Clear_Focus();
}

void GadgetClass::Disable(void) {
    IsDisabled = 1u;
    IsToRepaint = 1u;
    Clear_Focus();
}

GadgetClass* GadgetClass::Remove(void) {
    Clear_Focus();

    GadgetClass* head = this;
    while (head->Prev) {
        head = head->Prev;
        if (head == this) break;
    }

    GadgetClass* new_head = (head == this) ? Next : head;

    if (Prev) {
        Prev->Next = Next;
    }
    if (Next) {
        Next->Prev = Prev;
    }

    Prev = 0;
    Next = 0;

    return new_head;
}

void GadgetClass::Delete_List(void) {
    /*
    ** The portable port predominantly uses stack-allocated gadgets (see various dialogs).
    ** Keep this routine as a safe detach operation for now rather than calling delete.
    */
    GadgetClass* gadget = this;
    while (gadget->Prev) {
        gadget = gadget->Prev;
        if (gadget == this) break;
    }

    while (gadget) {
        GadgetClass* next = gadget->Next;
        gadget->Clear_Focus();
        gadget->Prev = 0;
        gadget->Next = 0;
        gadget = next;
    }
}

void GadgetClass::Flag_To_Redraw(void) { IsToRepaint = 1u; }

int GadgetClass::Draw_Me(int forced) {
    if (forced || IsToRepaint) {
        IsToRepaint = 0u;
        return 1;
    }
    return 0;
}

void GadgetClass::Draw_All(bool forced) {
    GadgetClass* gadget = this;
    while (gadget) {
        gadget->Draw_Me(forced ? 1 : 0);
        gadget = gadget->Next;
    }
}

void GadgetClass::Sticky_Process(unsigned flags) {
    if (IsSticky && (flags & LEFTPRESS)) {
        StuckOn = this;
    }
    if (StuckOn == this && (flags & LEFTRELEASE)) {
        StuckOn = 0;
    }
}

void GadgetClass::Set_Focus(void) {
    if (Focused) {
        Focused->Flag_To_Redraw();
        Focused->Clear_Focus();
    }
    Flags |= KEYBOARD;
    Focused = this;
}

void GadgetClass::Clear_Focus(void) {
    if (Focused == this) {
        Flags &= ~KEYBOARD;
        Focused = 0;
    }
}

bool GadgetClass::Has_Focus(void) { return this == Focused; }

int GadgetClass::Is_List_To_Redraw(void) {
    GadgetClass* gadget = this;
    while (gadget) {
        if (gadget->IsToRepaint) return 1;
        gadget = gadget->Next;
    }
    return 0;
}

KeyNumType GadgetClass::Input(void) {
    bool forced = false;

    if (LastList != this) {
        LastList = this;
        forced = true;
        StuckOn = 0;
        Focused = 0;
    }

    /*
    ** The legacy input system (Keyboard::Check/Get/Down and mouse queue state) is not wired up
    ** in the portable build yet. For now, keep gadget input as a safe no-op that still drives
    ** forced redraw behavior when the gadget list changes.
    */
    Draw_All(forced);
    return KN_NONE;
}

ControlClass* GadgetClass::Extract_Gadget(unsigned id) {
    if (!id) return 0;
    GadgetClass* gadget = this;
    while (gadget) {
        if (gadget->Get_ID() == id) return (ControlClass*)gadget;
        gadget = gadget->Next;
    }
    return 0;
}

KeyNumType TextButtonClass::Input()
{
    int key = SDL_Platform_Pop_Key();
    if (key) return (KeyNumType)key;

    if (!SDL_Platform_Mouse_Left_Pressed()) return KN_NONE;

    const int mx = Get_Mouse_X();
    const int my = Get_Mouse_Y();
    if (mx >= X && mx < (X + Width) && my >= Y && my < (Y + Height)) {
        return (KeyNumType)(Id | KN_BUTTON);
    }
    return KN_NONE;
}

void TextButtonClass::Draw_All(bool forced)
{
    if (!forced && !NeedsRedraw) return;
    NeedsRedraw = false;

    const int fill = IsOn ? CC_BRIGHT_GREEN : CC_GREEN_BKGD;
    const int border = CC_BRIGHT_GREEN;
    if (LogicPage) {
        LogicPage->Fill_Rect(X, Y, Width, Height, fill);
        LogicPage->Draw_Rect(X, Y, Width, Height, border);
    }

    char const* label = Text_String(Text);
    int text_w = String_Pixel_Width(label);
    int tx = X + (Width - text_w) / 2;
    int ty = Y + 2;
    Fancy_Text_Print(label, (unsigned)tx, (unsigned)ty, CC_GREEN, TBLACK, TextFlags);
}
