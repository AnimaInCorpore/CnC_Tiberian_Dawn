#include "function.h"
#include "checkbox.h"

int CheckBoxClass::Draw_Me(int forced) {
  if (!ToggleClass::Draw_Me(forced)) {
    return false;
  }

  if (LogicPage == &SeenBuff) {
    Conditional_Hide_Mouse(X, Y, X + Width - 1, Y + Height - 1);
  }
  Draw_Box(X, Y, Width, Height, BOXSTYLE_GREEN_DOWN, false);
  LogicPage->Fill_Rect(X + 1, Y + 1, X + Width - 2, Y + Height - 2, DKGREY);

  if (IsOn) {
    LogicPage->Draw_Line(X + 1, Y + 1, X + Width - 2, Y + Height - 2, BLACK);
    LogicPage->Draw_Line(X + Width - 2, Y + 1, X + 1, Y + Height - 2, BLACK);
  }

  if (LogicPage == &SeenBuff) {
    Conditional_Show_Mouse();
  }
  return true;
}
