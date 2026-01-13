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

#include "function.h"

#include "goptions.h"

void GameOptionsClass::Adjust_Variables_For_Resolution(void)
{
    int factor = (SeenBuff.Get_Width() == 320) ? 1 : 2;

    OptionWidth = (216 + 8) * factor;
    OptionHeight = 100 * factor;
    OptionX = ((SeenBuff.Get_Width() - OptionWidth) / 2);
    OptionY = ((SeenBuff.Get_Height() - OptionHeight) / 2);
    ButtonWidth = 130 * factor;
    OButtonHeight = 9 * factor;
    CaptionYPos = 5 * factor;
    ButtonY = 21 * factor;
    Border1Len = 72 * factor;
    Border2Len = 16 * factor;
    ButtonResumeY = (OptionHeight - (15 * factor));
}

void GameOptionsClass::Process(void)
{
    /*
    ** Stub: the full options dialog depends on `options.cpp` (not ported yet)
    ** and several other UI dialogs (load/save, sound, video, etc.).
    */
    Adjust_Variables_For_Resolution();
}

