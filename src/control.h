#pragma once

#include "gadget.h"

class ControlClass : public GadgetClass {
public:
    ControlClass(unsigned id,
                 int x,
                 int y,
                 int w,
                 int h,
                 unsigned flags = LEFTPRESS | RIGHTPRESS,
                 int sticky = false);

    virtual void Make_Peer(GadgetClass& gadget);

    virtual int Draw_Me(int forced = false);

    unsigned ID;

protected:
    virtual unsigned Get_ID(void) const;
    virtual int Action(unsigned flags, KeyNumType& key);

    GadgetClass* Peer;
};

