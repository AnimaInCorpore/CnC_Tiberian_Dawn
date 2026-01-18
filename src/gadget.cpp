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

int GadgetClass::Clicked_On(KeyNumType& key, unsigned flags, int mousex, int mousey)
{
    flags &= Flags;

    if (this == StuckOn || (flags & KEYBOARD) ||
        (flags && (unsigned)(mousex - X) < (unsigned)Width && (unsigned)(mousey - Y) < (unsigned)Height)) {
        return Action(flags, key);
    }

    return 0;
}

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
    int mousex;
    int mousey;
    KeyNumType key = KN_NONE;
    unsigned flags = 0;
    bool forced = false;

    if (LastList != this) {
        LastList = this;
        forced = true;
        StuckOn = 0;
        Focused = 0;
    }

    key = (KeyNumType)SDL_Platform_Pop_Key();

    const bool mouse_event = SDL_Platform_Mouse_Left_Pressed() || SDL_Platform_Mouse_Right_Pressed() ||
                             SDL_Platform_Mouse_Left_Released() || SDL_Platform_Mouse_Right_Released();
    if (mouse_event) {
        mousex = SDL_Platform_Mouse_Event_X();
        mousey = SDL_Platform_Mouse_Event_Y();
    } else {
        mousex = Get_Mouse_X();
        mousey = Get_Mouse_Y();
    }

    if (mouse_event) {
        if (SDL_Platform_Mouse_Left_Pressed()) flags |= LEFTPRESS;
        if (SDL_Platform_Mouse_Right_Pressed()) flags |= RIGHTPRESS;
        if (SDL_Platform_Mouse_Left_Released()) flags |= LEFTRELEASE;
        if (SDL_Platform_Mouse_Right_Released()) flags |= RIGHTRELEASE;
    }

    if (key) {
        flags |= KEYBOARD;
    }

    if (!key && !mouse_event) {
        if (SDL_Platform_Mouse_Left_Down()) {
            flags |= LEFTHELD;
        } else {
            flags |= LEFTUP;
        }

        if (SDL_Platform_Mouse_Right_Down()) {
            flags |= RIGHTHELD;
        } else {
            flags |= RIGHTUP;
        }
    }

    if (StuckOn) {
        StuckOn->Draw_Me(false);
        StuckOn->Clicked_On(key, flags, mousex, mousey);
        if (StuckOn) {
            StuckOn->Draw_Me(false);
        }
    } else {
        if (Focused && (flags & KEYBOARD)) {
            Focused->Draw_Me(false);
            Focused->Clicked_On(key, flags, mousex, mousey);
            if (Focused) {
                Focused->Draw_Me(false);
            }
        } else {
            GadgetClass* next_button = this;
            while (next_button) {
                next_button->Draw_Me(forced ? 1 : 0);

                if (!next_button->IsDisabled) {
                    if (next_button->Clicked_On(key, flags, mousex, mousey)) {
                        next_button->Draw_Me(false);
                        break;
                    }
                }

                next_button = next_button->Get_Next();
            }
        }
    }

    return key;
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
