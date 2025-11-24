#include "function.h"
#include "mapedit.h"
#include "tab.h"

const void* TabClass::TabShape = nullptr;

TabClass::TabClass() { IsToRedraw = false; }

void TabClass::Draw_It(bool complete) {
  SidebarClass::Draw_It(complete);

  if (Debug_Map) {
    return;
  }

  const int width = SeenBuff.Get_Width();
  const int rightx = width - 1;

  if (complete || IsToRedraw) {
    if (LogicPage->Lock()) {
      LogicPage->Fill_Rect(0, 0, rightx, Tab_Height - 2, BLACK);
      CC_Draw_Shape(TabShape, 0, 0, 0, WINDOW_MAIN, SHAPE_NORMAL);
      CC_Draw_Shape(TabShape, 0, width - Eva_Width, 0, WINDOW_MAIN, SHAPE_NORMAL);
      Draw_Credits_Tab();
      LogicPage->Draw_Line(0, Tab_Height - 1, rightx, Tab_Height - 1, BLACK);

      Fancy_Text_Print(TXT_TAB_BUTTON_CONTROLS, Eva_Width / 2, 0, 11, TBLACK,
                       TPF_GREEN12_GRAD | TPF_CENTER | TPF_USE_GRAD_PAL);
      Fancy_Text_Print(TXT_TAB_SIDEBAR, width - (Eva_Width / 2), 0, 11, TBLACK,
                       TPF_GREEN12_GRAD | TPF_CENTER | TPF_USE_GRAD_PAL);
    }
    LogicPage->Unlock();
  }

  Credits.Graphic_Logic(complete || IsToRedraw);
  IsToRedraw = false;
}

void TabClass::Draw_Credits_Tab() { CC_Draw_Shape(TabShape, 0, 320, 0, WINDOW_MAIN, SHAPE_NORMAL); }

void TabClass::Hilite_Tab(int /*tab*/) {
  const int text_x = 80;
  CC_Draw_Shape(TabShape, 1, 0, 0, WINDOW_MAIN, SHAPE_NORMAL);
  Fancy_Text_Print(TXT_TAB_BUTTON_CONTROLS, text_x, 0, 11, TBLACK,
                   TPF_GREEN12 | TPF_CENTER | TPF_USE_GRAD_PAL);
}

void TabClass::AI(KeyNumType& input, int x, int y) {
  if (y >= 0 && y < Tab_Height && x < (SeenBuff.Get_Width() - 1) && x > 0) {
    bool ok = false;
    const int width = SeenBuff.Get_Width();

    if (y > 0 || (Special.IsScrollMod && ((x > 3 && x < Eva_Width) ||
                                          (x < width - 3 && x > width - Eva_Width)))) {
      ok = true;
    }

    if (ok) {
      if (input == KN_LMOUSE) {
        int sel = -1;
        if (x < Eva_Width) {
          sel = 0;
        }
        if (x > width - Eva_Width) {
          sel = 1;
        }
        if (sel >= 0) {
          Set_Active(sel);
          input = KN_NONE;
        }
      }

      Override_Mouse_Shape(MOUSE_NORMAL, false);
    }
  }

  Credits.AI();
  SidebarClass::AI(input, x, y);
}

void TabClass::Set_Active(int select) {
 switch (select) {
    case 0:
      Queue_Options();
      break;
    case 1:
      Map.Activate(-1);
      break;
    default:
      break;
  }
}

void TabClass::One_Time() {
  const int factor = (SeenBuff.Get_Width() == 320) ? 1 : 2;
  Eva_Width = 80 * factor;
  Tab_Height = 8 * factor;

  SidebarClass::One_Time();
  TabShape = Hires_Retrieve("TABS.SHP");
}
