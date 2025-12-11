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

/* $Header:   F:\projects\c&c\vcs\code\credits.cpv   2.17   16 Oct 1995 16:51:28   JOE_BOSTIC  $ */
/***********************************************************************************************
 ***             C O N F I D E N T I A L  ---  W E S T W O O D   S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : Command & Conquer                                            *
 *                                                                                             *
 *                    File Name : CREDITS.CPP                                                  *
 *                                                                                             *
 *                   Programmer : Joe L. Bostic                                                *
 *                                                                                             *
 *                   Start Date : April 17, 1994                                               *
 *                                                                                             *
 *                  Last Update : March 13, 1995 [JLB]                                         *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 *   CreditClass::AI -- Handles updating the credit display.                                   *
 *   CreditClass::Graphic_Logic -- Handles the credit redraw logic.                            *
 *   CreditClass::CreditClass -- Default constructor for the credit class object.              *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "function.h"

#include <algorithm>
#include <cstdlib>

namespace {

constexpr int kCreditTabWidth = 120;
constexpr long kMaxStep = 71 + 72;

}  // namespace

/***********************************************************************************************
 * CreditClass::CreditClass -- Default constructor for the credit class object.                *
 *                                                                                             *
 *    This is the constructor for the credit class object. It merely sets the credit display   *
 *    state to null.                                                                           *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   03/13/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
CreditClass::CreditClass()
    : Credits(0),
      Current(0),
      IsToRedraw(false),
      IsUp(false),
      IsAudible(false),
      Countdown(0) {}

void CreditClass::Update(bool forced, bool redraw) {
  if (redraw) {
    IsToRedraw = true;
  }
  AI(forced);
  Graphic_Logic(forced || redraw);
}

/***********************************************************************************************
 * CreditClass::Graphic_Logic -- Handles the credit redraw logic.                              *
 *                                                                                             *
 *    This routine should be called whenever the main game screen is to be updated. It will    *
 *    check to see if the credit display should be redrawn. If so, it will redraw it.          *
 *                                                                                             *
 * INPUT:   forced   -- Should the credit display be redrawn regardless of whether the redraw  *
 *                      flag is set? This is typically the case when the screen needs to be    *
 *                      redrawn from scratch.                                                  *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   03/13/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
void CreditClass::Graphic_Logic(bool forced) {
  const int factor = Get_Resolution_Factor();
  const int xx = SeenBuff.Get_Width() - (kCreditTabWidth << factor);
  if (forced || IsToRedraw) {
    if (IsAudible) {
      Sound_Effect(IsUp ? VOC_UP : VOC_DOWN, VOL_1);
    }

    TabClass::Draw_Credits_Tab();
    Fancy_Text_Print("%ld", xx, 0, 11, TBLACK, TPF_GREEN12_GRAD | TPF_CENTER | TPF_USE_GRAD_PAL,
                     Current);

    IsToRedraw = false;
    IsAudible = false;
  }
}

/***********************************************************************************************
 * CreditClass::AI -- Handles updating the credit display.                                     *
 *                                                                                             *
 *    This routine handles the logic that controls the rate of credit change in the credit     *
 *    display. It doesn't actually redraw the credit display, but will flag it to be redrawn   *
 *    if it detects that a change is to occur.                                                 *
 *                                                                                             *
 * INPUT:   forced   -- Should the credit display immediately reflect the current credit       *
 *                      total for the player? This is usually desired when initially loading   *
 *                      a scenario or saved game.                                              *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   03/13/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
void CreditClass::AI(bool forced) {
  Credits = PlayerPtr->Available_Money();

  Credits = std::max(Credits, 0L);

  if (Current == Credits) return;

  if (forced) {
    IsAudible = false;
    Current = Credits;
  } else {
    if (Countdown) Countdown--;
    if (Countdown) return;

    long adder = Credits - Current;
    adder = std::abs(adder);
    adder >>= 5;
    adder = std::clamp(adder, 1L, kMaxStep);
    if (Current > Credits) adder = -adder;
    Current += adder;
    Countdown = 1;

    if (adder != 0) {
      IsAudible = true;
      IsUp = (adder > 0);
    }
  }
  IsToRedraw = true;
  Map.Flag_To_Redraw(false);
}

