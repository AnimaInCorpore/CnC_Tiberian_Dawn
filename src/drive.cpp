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

/* $Header:   F:\projects\c&c\vcs\code\drive.cpv   2.17   16 Oct 1995 16:51:16   JOE_BOSTIC  $ */
/***********************************************************************************************
 ***             C O N F I D E N T I A L  ---  W E S T W O O D   S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : Command & Conquer                                            *
 *                                                                                             *
 *                    File Name : DRIVE.CPP                                                    *
 *                                                                                             *
 *                   Programmer : Joe L. Bostic                                                *
 *                                                                                             *
 *                   Start Date : April 22, 1994                                               *
 *                                                                                             *
 *                  Last Update : July 30, 1995 [JLB]                                          *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 *   DriveClass::AI -- Processes unit movement and rotation.                                   *
 *   DriveClass::Approach_Target -- Handles approaching the target in order to attack it.      *
 *   DriveClass::Assign_Destination -- Set the unit's NavCom.                                  *
 *   DriveClass::Class_Of -- Fetches a reference to the class type for this object.            *
 *   DriveClass::Debug_Dump -- Displays status information to monochrome screen.               *
 *   DriveClass::Do_Turn -- Tries to turn the vehicle to the specified direction.              *
 *   DriveClass::DriveClass -- Constructor for drive class object.                             *
 *   DriveClass::Exit_Map -- Give the unit a movement order to exit the map.                   *
 *   DriveClass::Fixup_Path -- Adds smooth start path to normal movement path.                 *
 *   DriveClass::Force_Track -- Forces the unit to use the indicated track.                    *
 *   DriveClass::Lay_Track -- Handles track laying logic for the unit.                         *
 *   DriveClass::Offload_Tiberium_Bail -- Offloads one Tiberium quantum from the object.       *
 *   DriveClass::Ok_To_Move -- Checks to see if this object can begin moving.                  *
 *   DriveClass::Overrun_Square -- Handles vehicle overrun of a cell.                          *
 *   DriveClass::Per_Cell_Process -- Handles when unit finishes movement into a cell.          *
 *   DriveClass::Smooth_Turn -- Handles the low level coord calc for smooth turn logic.        *
 *   DriveClass::Start_Of_Move -- Tries to get a unit to advance toward cell.                  *
 *   DriveClass::Tiberium_Load -- Determine the Tiberium load as a percentage.                 *
 *   DriveClass::While_Moving -- Processes unit movement.                                      *
 *   DriveClass::Mark_Track -- Marks the midpoint of the track as occupied.                    *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "legacy/function.h"


DriveClass::DriveClass(void) : Class(0) {};


/***********************************************************************************************
 * DriveClass::Do_Turn -- Tries to turn the vehicle to the specified direction.                *
 *                                                                                             *
 *    This routine will set the vehicle to rotate to the direction specified. For tracked      *
 *    vehicles, it is just a simple rotation. For wheeled vehicles, it performs a series       *
 *    of short drives (three point turn) to face the desired direction.                        *
 *                                                                                             *
 * INPUT:   dir   -- The direction that this vehicle should face.                              *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   05/29/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
void DriveClass::Do_Turn(DirType dir)
{
    if (dir != PrimaryFacing) {

        /*
        **	Special rotation track is needed for units that
        **	cannot rotate in place.
        */
        if (Special.IsThreePoint && TrackNumber == -1 && Class->Speed == SPEED_WHEEL) {
            int            facediff;    // Signed difference between current and desired facing.
            FacingType    face;            // Current facing (ordinal value).

            facediff = PrimaryFacing.Difference(dir) >> 5;
            facediff = Bound(facediff, -2, 2);
            if (facediff) {
                face = Dir_Facing(PrimaryFacing);

                IsOnShortTrack = true;
                Force_Track(face*FACING_COUNT + (face + facediff), Coord);

                Path[0] = FACING_NONE;
                Set_Speed(0xFF);        // Full speed.
            }
        } else {
            PrimaryFacing.Set_Desired(dir);
            if (Special.IsJurassic && AreThingiesEnabled && What_Am_I() == RTTI_UNIT && ((UnitClass *)this)->Class->IsPieceOfEight) PrimaryFacing.Set_Current(dir);
        }
    }
}


/***********************************************************************************************
 * DriveClass::Force_Track -- Forces the unit to use the indicated track.                      *
 *                                                                                             *
 *    This override (nuclear bomb) style routine is to be used when a unit needs to start      *
 *    on a movement track but is outside the normal movement system. This occurs when a        *
 *    harvester starts driving off of a refinery.                                              *
 *                                                                                             *
 * INPUT:   track -- The track number to start on.                                             *
 *                                                                                             *
 *          coord -- The coordinate that the unit will end up at when the movement track       *
 *                   is completed.                                                             *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   03/17/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
void DriveClass::Force_Track(int track, COORDINATE coord)
{
    TrackNumber = track;
    TrackIndex = 0;
    Start_Driver(coord);
}


/***********************************************************************************************
 * DriveClass::Tiberium_Load -- Determine the Tiberium load as a percentage.                   *
 *                                                                                             *
 *    Use this routine to determine what the Tiberium load is (as a fixed point percentage).   *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Returns with the current "fullness" rating for the object. This will be 0x0000 for *
 *          empty and 0x0100 for full.                                                         *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   03/17/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
int DriveClass::Tiberium_Load(void) const
{
    if (*this == UNIT_HARVESTER) {
        return(Cardinal_To_Fixed(UnitTypeClass::STEP_COUNT, Tiberium));
    }
    return(0x0000);
}


/***********************************************************************************************
 * DriveClass::Approach_Target -- Handles approaching the target in order to attack it.        *
 *                                                                                             *
 *    This routine will check to see if the target is infantry and it can be overrun. It will  *
 *    try to overrun the infantry rather than attack it. This only applies to computer         *
 *    controlled vehicles. If it isn't the infantry overrun case, then it falls into the       *
 *    base class for normal (complex) approach algorithm.                                      *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   03/17/1995 JLB : Created.                                                                 *
 *   07/12/1995 JLB : Flamethrower tanks don't overrun -- their weapon is better.              *
 *=============================================================================================*/
void DriveClass::Approach_Target(void)
{
    /*
    **	Only if there is a legal target should the approach check occur.
    */
    if (!House->IsHuman && Target_Legal(TarCom) && !Target_Legal(NavCom)) {

        /*
        **	Special case:
        **	If this is for a unit that can crush infantry, and the target is
        **	infantry, AND the infantry is pretty darn close, then just try
        **	to drive over the infantry instead of firing on it.
        */
        TechnoClass * target = As_Techno(TarCom);
        if (Class->Primary != WEAPON_FLAME_TONGUE && Class->IsCrusher && Distance(TarCom) < 0x0180 && target && ((TechnoTypeClass const &)(target->Class_Of())).IsCrushable) {
            Assign_Destination(TarCom);
            return;
        }
    }

    /*
    **	In the other cases, uses the more complex "get to just within weapon range"
    **	algorithm.
    */
    FootClass::Approach_Target();
}


/*
**	(rest of original DRIVE.CPP contents copied verbatim)
*/
