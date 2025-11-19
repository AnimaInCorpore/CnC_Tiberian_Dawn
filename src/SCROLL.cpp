#include "function.h"
#include "scroll.h"

namespace {
constexpr int kScrollDelay = 1;
constexpr int kEvaWidth = 80;
constexpr int kEdgeBias = 50;
}  // namespace

CountDownTimerClass ScrollClass::Counter;

ScrollClass::ScrollClass() : IsAutoScroll(true), Inertia(0) { Counter.Set(kScrollDelay); }

void ScrollClass::AI(KeyNumType& input, int x, int y) {
  static DirType direction;
  bool player_scrolled = false;

  if (!IsRubberBand) {
    bool noscroll = false;
    if (Special.IsScrollMod && y == 0) {
      const int right_edge = SeenBuff.Get_Width() - 1;
      if ((x > 3 && x < kEvaWidth) || (x > right_edge - kEvaWidth && x < right_edge - 2)) {
        noscroll = true;
      }
    }

    if (!noscroll) {
      const int max_x = SeenBuff.Get_Width() - 1;
      const int max_y = SeenBuff.Get_Height() - 1;
      if (Inertia || y == 0 || x == 0 || x == max_x || y == max_y) {
        if (y == 0 || x == 0 || x == max_x || y == max_y) {
          player_scrolled = true;

          int altx = x;
          if (altx < kEdgeBias) {
            altx -= (kEdgeBias - altx) * 2;
          }
          if (altx < 0) {
            altx = 0;
          }
          if (altx > max_x - (kEdgeBias - 1)) {
            altx += (altx - (max_x - (kEdgeBias - 1))) * 2;
          }
          if (altx > SeenBuff.Get_Width()) {
            altx = SeenBuff.Get_Width();
          }
          if (altx > kEdgeBias && altx < (SeenBuff.Get_Width() - kEdgeBias)) {
            altx += ((SeenBuff.Get_Width() / 2) - altx) / 2;
          }

          int alty = y;
          if (alty < kEdgeBias) {
            alty -= (kEdgeBias - alty);
          }
          if (alty < 0) {
            alty = 0;
          }
          if (alty > max_y - (kEdgeBias - 1)) {
            alty += (alty - (max_y - (kEdgeBias - 1)));
          }
          if (alty > SeenBuff.Get_Height()) {
            alty = SeenBuff.Get_Height();
          }

          direction = static_cast<DirType>(Desired_Facing256(SeenBuff.Get_Width() / 2, SeenBuff.Get_Height() / 2, altx, alty));
        }

        const int control = Dir_Facing(direction);
        static constexpr int kRates[] = {
            0x01C0, 0x0180, 0x0140, 0x0100, 0x00C0, 0x0080, 0x0040, 0x0020, 0x0010,
        };

        int rate = 8 - Inertia;
        const int min_rate = Options.ScrollRate + 1;
        if (rate < min_rate) {
          rate = min_rate;
          Inertia = 8 - rate;
        }

        if (Keyboard::Down(KN_RMOUSE)) {
          rate = Bound(rate + 1, 4, static_cast<int>(sizeof(kRates) / sizeof(kRates[0])) - 1);
        }

        if (!Options.IsFreeScroll) {
          direction = Facing_Dir(Dir_Facing(direction));
        }

        int distance = kRates[rate] / 2;
        if (!Scroll_Map(direction, distance, false)) {
          Override_Mouse_Shape(static_cast<MouseType>(MOUSE_NO_N + control), false);
        } else {
          Override_Mouse_Shape(static_cast<MouseType>(MOUSE_N + control), false);

          if (Keyboard::Down(KN_LMOUSE) || IsAutoScroll) {
            distance = kRates[rate];
            Scroll_Map(direction, distance, true);

            if (Counter.Time() == 0 && player_scrolled) {
              Counter.Set(kScrollDelay);
              ++Inertia;
            }
          }
        }
      }

      if (!player_scrolled && Counter.Time() == 0) {
        --Inertia;
        if (Inertia < 0) {
          ++Inertia;
        }
        Counter.Set(kScrollDelay);
      }
    }
  }

  HelpClass::AI(input, x, y);
}

bool ScrollClass::Set_Autoscroll(int control) {
  bool old = IsAutoScroll;

  switch (control) {
    case -1:
      IsAutoScroll = !IsAutoScroll;
      break;
    default:
      IsAutoScroll = (control != 0);
      break;
  }
  return old;
}
