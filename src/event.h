#pragma once

#include "legacy_compat.h"

/*
**	This event class is used to contain all external game events (things that the player can
**	do at any time) so that these events can be transported between linked computers. This
**	encapsulation is required in order to ensure that each event affects all computers at the
**	same time (same game frame).
*/
class EventClass
{
public:
    /*
    **	All external events are identified by these labels.
    */
    typedef enum EventType {
        EMPTY,

        ALLY,             // Make allie of specified house.
        MEGAMISSION,      // Full change of mission with target and destination.
        IDLE,             // Request to enter idle mode.
        SCATTER,          // Request to scatter from current location.
        DESTRUCT,         // Self destruct request (surrender action).
        DEPLOY,           // MCV is to deploy at current location.
        PLACE,            // Place building at location specified.
        OPTIONS,          // Bring up options screen.
        GAMESPEED,        // Set game speed
        PRODUCE,          // Start or Resume production.
        SUSPEND,          // Suspend production.
        ABANDON,          // Abandon production.
        PRIMARY,          // Primary factory selected.
        SPECIAL_PLACE,    // Special target location selected
        EXIT,             // Exit game.
        ANIMATION,        // Flash ground as movement feedback.
        REPAIR,           // Repair specified object.
        SELL,             // Sell specified object.
        SPECIAL,          // Special options control.

        // Private events.
        FRAMESYNC,        // Game-connection packet.
        MESSAGE,          // Message to another player.
        RESPONSE_TIME,    // use a new propogation delay value
        FRAMEINFO,        // Game-heartbeat packet.
        TIMING,           // new timing values for all systems to use
        PROCESS_TIME,     // a system's average processing time, in ticks per frame
        LAST_EVENT,       // one past the last event
    } EventType;

    EventType Type; // Type of queue command object.

    /*
    ** 'Frame' is the frame that the command should execute on.
    */
    unsigned Frame : 27;

    /*
    ** House index of the player originating this event
    */
    unsigned ID : 4;

    /*
    ** This bit tells us if we've already executed this event.
    */
    unsigned IsExecuted : 1;

    /*
    ** Multiplayer ID of the player originating this event.
    */
    unsigned char MPlayerID;

    /*
    **	This union contains the specific data that the event requires.
    */
    union {
        struct {
            SpecialClass Data; // The special option flags.
        } Options;
        struct {
            TARGET Whom; // The object to apply the event to.
        } Target;
        struct {
            AnimType What;      // The animation to create.
            HousesType Owner;   // The owner of the animation (when it matters).
            COORDINATE Where;   // The location to place the animation.
        } Anim;
        struct {
            int Value; // general-purpose data
        } General;
        struct {
            TARGET Whom;         // Whom to apply mission to.
            MissionType Mission; // What mission to apply.
            TARGET Target;       // Target to assign.
            TARGET Destination;  // Destination to assign.
        } MegaMission;
        struct {
            TARGET Whom;         // Whom to apply mission to.
            MissionType Mission; // What mission to apply.
        } Mission;
        struct {
            TARGET Whom;  // Whom to apply movement change to.
            TARGET Where; // Where to set NavCom to.
        } NavCom;
        struct {
            TARGET Whom;   // Whom to apply attack change to.
            TARGET Target; // What to set TarCom to.
        } TarCom;
        struct {
            RTTIType Type;
            int ID;
        } Specific;
        struct {
            RTTIType Type;
            CELL Cell;
        } Place;
        struct {
            int ID;
            CELL Cell;
        } Special;
        struct {
            unsigned long CRC;
            unsigned short CommandCount;
            unsigned char Delay;
        } FrameInfo;
        struct {
            unsigned short DesiredFrameRate;
            unsigned short MaxAhead;
        } Timing;
        struct {
            unsigned short AverageTicks;
        } ProcessTime;
    } Data;

    //-------------- Functions ---------------------
    EventClass(void) { Type = EMPTY; }
    EventClass(SpecialClass data);
    EventClass(EventType type, TARGET target);
    EventClass(EventType type);
    EventClass(EventType type, int val);
    EventClass(EventType type, TARGET src, TARGET dest);
    EventClass(TARGET src, MissionType mission, TARGET target = TARGET_NONE, TARGET destination = TARGET_NONE);
    EventClass(EventType type, RTTIType object, int id);
    EventClass(EventType type, RTTIType object, CELL cell);
    EventClass(EventType type, int id, CELL cell);
    EventClass(AnimType anim, HousesType owner, COORDINATE coord);

    // Process the event.
    void Execute(void);

    int operator==(EventClass& q) { return memcmp(this, &q, sizeof(q)) == 0; }

    static unsigned char EventLength[LAST_EVENT];
    static char const* EventNames[LAST_EVENT];
};

