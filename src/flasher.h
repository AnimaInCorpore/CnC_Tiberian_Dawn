#pragma once

#ifdef CHEAT_KEYS
class MonoClass;
#endif

class FlasherClass {
public:
    /*
    **  When this object is targeted, it will flash a number of times. This is the
    **  flash control number. It counts down to zero and then stops. Odd values
    **  cause the object to be rendered in a lighter color.
    */
    unsigned FlashCount : 7;

    /*
    **  When an object is targeted, it flashes several times to give visual feedback
    **  to the player. Every other game "frame", this flag is true until the flashing
    **  is determined to be completed.
    */
    unsigned IsBlushing : 1;

    FlasherClass(void) : FlashCount(0), IsBlushing(0) {}

#ifdef CHEAT_KEYS
    void Debug_Dump(MonoClass* mono) const;
#endif

    bool Process(void);

    void Code_Pointers(void);
    void Decode_Pointers(void);
};

