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

/* $Header:   F:\\projects\\c&c\\vcs\\code\\control.cpv   2.18   16 Oct 1995 16:51:38   JOE_BOSTIC  $ */

#include "control.h"

ControlClass::ControlClass(unsigned id, int x, int y, int w, int h, unsigned flags, int sticky)
    : GadgetClass(x, y, w, h, flags, sticky), ID(id), Peer(NULL) {}

int ControlClass::Action(unsigned flags, KeyNumType& key) {
    if (Peer) {
        Peer->Peer_To_Peer(flags, key, *this);
    }

    if (flags) {
        if (ID) {
            key = (KeyNumType)(ID | KN_BUTTON);
        } else {
            key = KN_NONE;
        }
    }

    return GadgetClass::Action(flags, key);
}

void ControlClass::Make_Peer(GadgetClass& gadget) { Peer = &gadget; }

unsigned ControlClass::Get_ID(void) const { return ID; }

int ControlClass::Draw_Me(int forced) {
    if (Peer) {
        Peer->Draw_Me();
    }
    return GadgetClass::Draw_Me(forced);
}

