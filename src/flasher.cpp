#include "flasher.h"

#ifdef CHEAT_KEYS
void FlasherClass::Debug_Dump(MonoClass* mono) const { (void)mono; }
#endif

bool FlasherClass::Process(void) {
    if (FlashCount) {
        --FlashCount;
        IsBlushing = 0;

        if (FlashCount & 0x01) {
            IsBlushing = 1;
        }
        return true;
    }
    return false;
}

