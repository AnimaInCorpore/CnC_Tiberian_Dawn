#include "function.h"
#include "event.h"
#include "map.h"

static unsigned Event_Player_ID()
{
    if (!PlayerPtr) return 0;
    return static_cast<unsigned>(PlayerPtr->Class->House) & 0x0Fu;
}

/*
** Table of what data is really used in the EventClass struct for different
** events. This table must be kept current with the EventType enum.
*/
unsigned char EventClass::EventLength[EventClass::LAST_EVENT] = {
    0,                                 // EMPTY
    size_of(EventClass, Data.General),  // ALLY
    size_of(EventClass, Data.MegaMission), // MEGAMISSION
    size_of(EventClass, Data.Target),   // IDLE
    size_of(EventClass, Data.Target),   // SCATTER
    0,                                 // DESTRUCT
    0,                                 // DEPLOY
    size_of(EventClass, Data.Place),    // PLACE
    0,                                 // OPTIONS
    size_of(EventClass, Data.General),  // GAMESPEED
    size_of(EventClass, Data.Specific), // PRODUCE
    size_of(EventClass, Data.Specific.Type), // SUSPEND
    size_of(EventClass, Data.Specific.Type), // ABANDON
    size_of(EventClass, Data.Target),   // PRIMARY
    size_of(EventClass, Data.Special),  // SPECIAL_PLACE
    0,                                 // EXIT
    size_of(EventClass, Data.Anim),     // ANIMATION
    size_of(EventClass, Data.Target),   // REPAIR
    size_of(EventClass, Data.Target),   // SELL
    size_of(EventClass, Data.Options),  // SPECIAL
    0,                                 // FRAMESYNC
    0,                                 // MESSAGE
    size_of(EventClass, Data.FrameInfo.Delay), // RESPONSE_TIME
    size_of(EventClass, Data.FrameInfo), // FRAMEINFO
    size_of(EventClass, Data.Timing),   // TIMING
    size_of(EventClass, Data.ProcessTime), // PROCESS_TIME
};

char const* EventClass::EventNames[EventClass::LAST_EVENT] = {
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
    ID = Event_Player_ID();
    Type = SPECIAL;
    Frame = ::Frame;
    IsExecuted = 0;
    MPlayerID = 0;
    Data.Options.Data = data;
}

EventClass::EventClass(EventType type, TARGET target)
{
    ID = Event_Player_ID();
    Type = type;
    Frame = ::Frame;
    IsExecuted = 0;
    MPlayerID = 0;
    Data.Target.Whom = target;
}

EventClass::EventClass(EventType type)
{
    ID = Event_Player_ID();
    Type = type;
    Frame = ::Frame;
    IsExecuted = 0;
    MPlayerID = 0;
}

EventClass::EventClass(EventType type, int val)
{
    ID = Event_Player_ID();
    Type = type;
    Frame = ::Frame;
    IsExecuted = 0;
    MPlayerID = 0;
    Data.General.Value = val;
}

EventClass::EventClass(EventType type, TARGET src, TARGET dest)
{
    ID = Event_Player_ID();
    Type = type;
    Frame = ::Frame;
    IsExecuted = 0;
    MPlayerID = 0;
    Data.NavCom.Whom = src;
    Data.NavCom.Where = dest;
}

EventClass::EventClass(AnimType anim, HousesType owner, COORDINATE coord)
{
    ID = Event_Player_ID();
    Type = ANIMATION;
    Frame = ::Frame;
    IsExecuted = 0;
    MPlayerID = 0;
    Data.Anim.What = anim;
    Data.Anim.Owner = owner;
    Data.Anim.Where = coord;
}

EventClass::EventClass(TARGET src, MissionType mission, TARGET target, TARGET destination)
{
    ID = Event_Player_ID();
    Type = MEGAMISSION;
    Frame = ::Frame;
    IsExecuted = 0;
    MPlayerID = 0;
    Data.MegaMission.Whom = src;
    Data.MegaMission.Mission = mission;
    Data.MegaMission.Target = target;
    Data.MegaMission.Destination = destination;
}

EventClass::EventClass(EventType type, RTTIType object, int id)
{
    ID = Event_Player_ID();
    Type = type;
    Frame = ::Frame;
    IsExecuted = 0;
    MPlayerID = 0;
    Data.Specific.Type = object;
    Data.Specific.ID = id;
}

EventClass::EventClass(EventType type, RTTIType object, CELL cell)
{
    ID = Event_Player_ID();
    Type = type;
    Frame = ::Frame;
    IsExecuted = 0;
    MPlayerID = 0;
    Data.Place.Type = object;
    Data.Place.Cell = cell;
}

EventClass::EventClass(EventType type, int id, CELL cell)
{
    ID = Event_Player_ID();
    Type = type;
    Frame = ::Frame;
    IsExecuted = 0;
    MPlayerID = 0;
    Data.Special.ID = id;
    Data.Special.Cell = cell;
}

void EventClass::Execute(void)
{
    switch (Type) {
        case SPECIAL:
            Special = Data.Options.Data;
            Map.Flag_To_Redraw(false);
            break;

        case EXIT:
            GameActive = false;
            break;

        default:
            break;
    }

    IsExecuted = 1;
}
