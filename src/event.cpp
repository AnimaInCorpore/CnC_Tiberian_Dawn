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
#include "ccdde.h"

unsigned char EventClass::EventLength[EventClass::LAST_EVENT] = {
    0,                                                          // EMPTY
    size_of(EventClass, Data.General ),                // ALLY
    size_of(EventClass, Data.MegaMission ),        // MEGAMISSION
    size_of(EventClass, Data.Target ),                // IDLE
    size_of(EventClass, Data.Target ),                // SCATTER
    0,                                                          // DESTRUCT
    0,                                                          // DEPLOY
    size_of(EventClass, Data.Place ),                // PLACE
    0,                                                          // OPTIONS
    size_of(EventClass, Data.General ),                // GAMESPEED
    size_of(EventClass, Data.Specific ),            // PRODUCE
    size_of(EventClass, Data.Specific.Type ),        // SUSPEND
    size_of(EventClass, Data.Specific.Type ),        // ABANDON
    size_of(EventClass, Data.Target ),                // PRIMARY
    size_of(EventClass, Data.Special ),                // SPECIAL_PLACE
    0,                                                          // EXIT
    size_of(EventClass, Data.Anim ),                    // ANIMATION
    size_of(EventClass, Data.Target ),                // REPAIR
    size_of(EventClass, Data.Target ),                // SELL
    size_of(EventClass, Data.Options ),                // SPECIAL
    0,                                                          // FRAMESYNC
    0,                                                          //	MESSAGE
    size_of(EventClass, Data.FrameInfo.Delay ),    // RESPONSE_TIME
    size_of(EventClass, Data.FrameInfo ),            // FRAMEINFO
    size_of(EventClass, Data.Timing ),                // TIMING
    size_of(EventClass, Data.ProcessTime ),        // PROCESS_TIME
};

char * EventClass::EventNames[EventClass::LAST_EVENT] = {
    "EMPTY",
    "ALLY",
    "MEGAMISSION",
    "IDLE",
    "SCATTER",
    "DESTRUCT",
    "DEPLOY",
    "PLACE",
    "OPTIONS",
    "GAMESPEED",
    "PRODUCE",
    "SUSPEND",
    "ABANDON",
    "PRIMARY",
    "SPECIAL_PLACE",
    "EXIT",
    "ANIMATION",
    "REPAIR",
    "SELL",
    "SPECIAL",
    "FRAMESYNC",
    "MESSAGE",
    "RESPONSE_TIME",
    "FRAMEINFO",
    "TIMING",
    "PROCESS_TIME",
};

EventClass::EventClass(SpecialClass data)
{
    ID = Houses.ID(PlayerPtr);
    Type = SPECIAL;
    Frame = ::Frame;
    Data.Options.Data = data;
}

EventClass::EventClass(EventType type, TARGET target)
{
    ID = Houses.ID(PlayerPtr);
    Type = type;
    Frame = ::Frame;
    Data.Target.Whom = target;
}

EventClass::EventClass(EventType type)
{
    ID = Houses.ID(PlayerPtr);
    Type = type;
    Frame = ::Frame;
}

EventClass::EventClass(EventType type, int val)
{
    ID = Houses.ID(PlayerPtr);
    Type = type;
    Data.General.Value = val;
    Frame = ::Frame;
}

EventClass::EventClass(EventType type, TARGET src, TARGET dest)
{
    ID = Houses.ID(PlayerPtr);
    Type = type;
    Frame = ::Frame;
    Data.NavCom.Whom = src;
    Data.NavCom.Where = dest;
}

EventClass::EventClass(AnimType anim, HousesType owner, COORDINATE coord)
{
    ID = Houses.ID(PlayerPtr);
    Type = ANIMATION;
    Frame = ::Frame;
    Data.Anim.What = anim;
    Data.Anim.Owner = owner;
    Data.Anim.Where = coord;
}

EventClass::EventClass(TARGET src, MissionType mission, TARGET target, TARGET destination)
{
    ID = Houses.ID(PlayerPtr);
    Type = MEGAMISSION;
    Frame = ::Frame;
    Data.MegaMission.Whom = src;
    Data.MegaMission.Mission = mission;
    Data.MegaMission.Target = target;
    Data.MegaMission.Destination = destination;
}
