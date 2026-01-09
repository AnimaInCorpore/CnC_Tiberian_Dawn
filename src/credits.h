#pragma once

#include "legacy_compat.h"

class CreditClass {
public:
    long Credits;
    long Current;

    unsigned IsToRedraw : 1;
    unsigned IsUp : 1;
    unsigned IsAudible : 1;

    CreditClass(void);

    void Update(bool forced = false, bool redraw = false);
    void Graphic_Logic(bool forced = false);
    void AI(bool forced = false);

private:
    int Countdown;
};

