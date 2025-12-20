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
#include <cstdio>

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

char const * EventClass::EventNames[EventClass::LAST_EVENT] = {
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

EventClass::EventClass(EventType type, RTTIType object, int id)
{
    ID = Houses.ID(PlayerPtr);
    Type = type;
    Frame = ::Frame;
    Data.Specific.Type = object;
    Data.Specific.ID = id;
}

EventClass::EventClass(EventType type, RTTIType object, CELL cell)
{
    ID = Houses.ID(PlayerPtr);
    Type = type;
    Frame = ::Frame;
    Data.Place.Type = object;
    Data.Place.Cell = cell;
}

EventClass::EventClass(EventType type, int id, CELL cell)
{
    ID = Houses.ID(PlayerPtr);
    Type = type;
    Frame = ::Frame;
    Data.Special.ID = id;
    Data.Special.Cell = cell;
}

void EventClass::Execute(void)
{
    TechnoClass * techno = nullptr;
    AnimClass * anim = nullptr;
    HouseClass * house = nullptr;
    char txt[80];
    int i;

    if (Type < 0 || Type > PROCESS_TIME) {
        char tempbuf[128];
        std::snprintf(tempbuf, sizeof(tempbuf), "Packet type %d received\n", Type);
        CCDebugString(tempbuf);

        std::snprintf(tempbuf, sizeof(tempbuf), "	ID = %d\n", ID);
        CCDebugString(tempbuf);

        std::snprintf(tempbuf, sizeof(tempbuf), "	Frame = %d\n", Frame);
        CCDebugString(tempbuf);

        std::snprintf(tempbuf, sizeof(tempbuf), "	MPlayer ID = %d\n", MPlayerID);
        CCDebugString(tempbuf);
    }

    switch (Type) {
        case ALLY:
            house = Houses.Raw_Ptr(Data.General.Value);
            if (Houses.Raw_Ptr(ID)->Is_Ally(house)) {
                Houses.Raw_Ptr(ID)->Make_Enemy((HousesType)Data.General.Value);
            } else {
                Houses.Raw_Ptr(ID)->Make_Ally((HousesType)Data.General.Value);
            }
            break;

        case DESTRUCT:
            CCDebugString("C&C95 - Resignation packet received\n");
            Houses.Raw_Ptr(ID)->Flag_To_Die();
            Houses.Raw_Ptr(ID)->Resigned = true;
            break;

        case SPECIAL:
            Special = Data.Options.Data;
            house = Houses.Raw_Ptr(ID);
            std::snprintf(txt, sizeof(txt), Text_String(TXT_SPECIAL_WARNING), house->Name);
            Messages.Add_Message(txt, MPlayerTColors[house->RemapColor],
                                 TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_FULLSHADOW, 1200, 0, 0);
            Map.Flag_To_Redraw(false);
            break;

        case REPAIR:
            CCDebugString("C&C95 - Repair packet received\n");
            techno = As_Techno(Data.Target.Whom);
            if (techno && techno->IsActive) {
                techno->Repair(-1);
            }
            break;

        case SELL:
            CCDebugString("C&C95 - Sell packet received\n");
            techno = As_Techno(Data.Target.Whom);
            if (techno && techno->IsActive && techno->House == Houses.Raw_Ptr(ID)) {
                techno->Sell_Back(-1);
            } else if (Is_Target_Cell(Data.Target.Whom)) {
                Houses.Raw_Ptr(ID)->Sell_Wall(As_Cell(Data.Target.Whom));
            }
            break;

        case ANIMATION:
            anim = new AnimClass(Data.Anim.What, Data.Anim.Where);
            if (anim) {
                if (Data.Anim.Owner != HOUSE_NONE && PlayerPtr->Class->House != Data.Anim.Owner &&
                    !Special.IsVisibleTarget) {
                    anim->Make_Invisible();
                }
            }
            break;

        case PLACE:
            CCDebugString("C&C95 - Place packet received\n");
            Houses.Raw_Ptr(ID)->Place_Object(Data.Place.Type, Data.Place.Cell);
            break;

        case PRODUCE:
            CCDebugString("C&C95 - Produce packet received\n");
            Houses.Raw_Ptr(ID)->Begin_Production(Data.Specific.Type, Data.Specific.ID);
            break;

        case SUSPEND:
            CCDebugString("C&C95 - Suspend packet received\n");
            Houses.Raw_Ptr(ID)->Suspend_Production(Data.Specific.Type);
            break;

        case ABANDON:
            CCDebugString("C&C95 - Abandon packet received\n");
            Houses.Raw_Ptr(ID)->Abandon_Production(Data.Specific.Type);
            break;

        case PRIMARY: {
            CCDebugString("C&C95 - Primary building packet received\n");
            BuildingClass * building = As_Building(Data.Target.Whom);
            if (building && building->IsActive) {
                building->Toggle_Primary();
            }
            break;
        }

        case MEGAMISSION:
            techno = As_Techno(Data.MegaMission.Whom);
            if (techno && techno->IsActive) {
                ObjectClass * object = nullptr;
                if (Target_Legal(Data.MegaMission.Target)) {
                    object = As_Object(Data.MegaMission.Target);
                } else {
                    object = As_Object(Data.MegaMission.Destination);
                }

                if (!techno->IsTethered) {
                    techno->Transmit_Message(RADIO_OVER_OUT);
                }

                switch (techno->What_Am_I()) {
                    case RTTI_INFANTRY:
                    case RTTI_UNIT:
                        if (static_cast<FootClass *>(techno)->Team) {
                            static_cast<FootClass *>(techno)->Team->Remove(static_cast<FootClass *>(techno));
                        }
                        break;
                }

                if (object) {
                    if (PlayerPtr->Is_Ally(techno) || Special.IsVisibleTarget) {
                        object->Clicked_As_Target();
                    }
                }

                techno->Assign_Mission(Data.MegaMission.Mission);

                if (Data.MegaMission.Mission == MISSION_GUARD_AREA &&
                    (techno->What_Am_I() == RTTI_INFANTRY || techno->What_Am_I() == RTTI_UNIT ||
                     techno->What_Am_I() == RTTI_AIRCRAFT)) {
                    static_cast<FootClass *>(techno)->ArchiveTarget = Data.MegaMission.Target;
                    techno->Assign_Target(TARGET_NONE);
                    techno->Assign_Destination(Data.MegaMission.Target);
                } else {
                    techno->Assign_Target(Data.MegaMission.Target);
                    techno->Assign_Destination(Data.MegaMission.Destination);
                }
            }
            break;

        case IDLE:
            techno = As_Techno(Data.Target.Whom);
            if (techno && techno->IsActive && !techno->IsInLimbo && !techno->IsTethered) {
                techno->Assign_Destination(TARGET_NONE);
                techno->Assign_Target(TARGET_NONE);
                techno->Enter_Idle_Mode();
            }
            break;

        case SCATTER:
            techno = As_Techno(Data.Target.Whom);
            if (techno && techno->IsActive && !techno->IsInLimbo && !techno->IsTethered) {
                techno->Scatter(0, true);
            }
            break;

        case SPECIAL_PLACE:
            CCDebugString("C&C95 - Special blast packet received\n");
            Houses.Raw_Ptr(ID)->Place_Special_Blast((SpecialWeaponType)Data.Special.ID, Data.Special.Cell);
            break;

        case EXIT:
            CCDebugString("C&C95 - Exit game packet received\n");
            Theme.Queue_Song(THEME_NONE);
            Stop_Speaking();
            Speak(VOX_CONTROL_EXIT);
            while (Is_Speaking()) {
                Call_Back();
            }
            GameActive = false;
            break;

        case OPTIONS:
            SpecialDialog = SDLG_OPTIONS;
            break;

        case GAMESPEED:
            CCDebugString("C&C95 - Game speed packet received\n");
            Options.GameSpeed = Data.General.Value;
            break;

        case RESPONSE_TIME: {
            char flip[128];
            std::snprintf(flip, sizeof(flip), "C&C95 - Changing MaxAhead to %d frames\n", Data.FrameInfo.Delay);
            CCDebugString(flip);
            MPlayerMaxAhead = Data.FrameInfo.Delay;
            break;
        }

        case TIMING: {
            char flip[128];
            CCDebugString("C&C95 - Timing packet received\n");
            if (Data.Timing.MaxAhead > MPlayerMaxAhead) {
                NewMaxAheadFrame1 = Frame;
                NewMaxAheadFrame2 = Frame + Data.Timing.MaxAhead;
            }

            DesiredFrameRate = Data.Timing.DesiredFrameRate;
            MPlayerMaxAhead = Data.Timing.MaxAhead;

            std::snprintf(flip, sizeof(flip), "C&C95 -  Timing packet: DesiredFrameRate = %d\n",
                          Data.Timing.DesiredFrameRate);
            CCDebugString(flip);
            std::snprintf(flip, sizeof(flip), "C&C95 -  Timing packet: MaxAhead = %d\n",
                          Data.Timing.MaxAhead);
            CCDebugString(flip);

            if (Special.IsFromWChat) {
                MPlayerMaxAhead += DDEServer.Time_Since_Heartbeat() / (70 * 60);
            }
            break;
        }

        case PROCESS_TIME: {
            for (i = 0; i < MPlayerCount; i++) {
                if (MPlayerID == ::MPlayerID[i]) {
                    TheirProcessTime[i] = Data.ProcessTime.AverageTicks;
                    char flip[128];
                    std::snprintf(flip, sizeof(flip),
                                  "C&C95 - Received PROCESS_TIME packet of %04x ticks\n",
                                  Data.ProcessTime.AverageTicks);
                    CCDebugString(flip);
                    break;
                }
            }
            break;
        }

        default:
            break;
    }
}
