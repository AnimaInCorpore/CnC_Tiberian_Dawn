#include "credits.h"
#include "tab.h"

#include "audio.h"
#include "legacy_compat.h"

CreditClass::CreditClass(void) {
    IsToRedraw = false;
    IsUp = false;
    IsAudible = false;
    Credits = 0;
    Current = 0;
    Countdown = 0;
}

void CreditClass::Update(bool forced, bool redraw) {
    AI(forced);
    Graphic_Logic(redraw);
}

void CreditClass::Graphic_Logic(bool forced) {
    int factor = Get_Resolution_Factor();
    int xx = SeenBuff.Get_Width() - (120 << factor);
    if (forced || IsToRedraw) {
        if (IsAudible) {
            if (IsUp) {
                Sound_Effect(VOC_UP, VOL_1);
            } else {
                Sound_Effect(VOC_DOWN, VOL_1);
            }
        }

        TabClass::Draw_Credits_Tab();
        Fancy_Text_Print("%ld",
                         xx,
                         0,
                         11,
                         TBLACK,
                         static_cast<TextPrintType>(TPF_GREEN12_GRAD | TPF_CENTER | TPF_USE_GRAD_PAL),
                         Current);

        IsToRedraw = false;
        IsAudible = false;
    }
}

void CreditClass::AI(bool forced) {
    if (PlayerPtr) {
        Credits = PlayerPtr->Available_Money();
    } else {
        Credits = 0;
    }

    Credits = MAX(Credits, 0L);

    if (Current == Credits) return;

    if (forced) {
        IsAudible = false;
        Current = Credits;
    } else {
        if (Countdown) Countdown--;
        if (Countdown) return;

        long adder = Credits - Current;
        adder = ABS(adder);
        adder >>= 5;
        adder = Bound(adder, 1L, 71 + 72);
        if (Current > Credits) adder = -adder;
        Current += adder;
        Countdown = 1;

        if (Current - adder != Current) {
            IsAudible = true;
            IsUp = (adder > 0);
        }
    }
    IsToRedraw = true;
    Map.Flag_To_Redraw(false);
}

