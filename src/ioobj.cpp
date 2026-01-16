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

/* $Header:   F:\\projects\\c&c\\vcs\\code\\ioobj.cpv   2.18   16 Oct 1995 16:51:22   JOE_BOSTIC  $ */

#include "function.h"

#include "factory.h"

/*
**  Save/load support is not wired up yet (saveload.cpp + pointer coding tables).
**  Provide stable, side-effect-free stubs so higher-level systems can link.
*/
bool FactoryClass::Load(FileClass& file) {
    (void)file;
    return true;
}

bool FactoryClass::Save(FileClass& file) {
    (void)file;
    return true;
}

void FactoryClass::Code_Pointers(void) {}

void FactoryClass::Decode_Pointers(void) {}

