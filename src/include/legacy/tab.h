#pragma once

#include "credits.h"
#include "sidebar.h"

class TabClass : public SidebarClass {
 public:
  TabClass();

  void AI(KeyNumType& input, int x, int y) override;
  void Draw_It(bool complete = false) override;
  void One_Time() override;

  static void Draw_Credits_Tab();
  static void Hilite_Tab(int tab);
  void Redraw_Tab() {
    IsToRedraw = true;
    Flag_To_Redraw(false);
  }

  void Code_Pointers() override;
  void Decode_Pointers() override;
  int Get_Tab_Height() const { return Tab_Height; }

  CreditClass Credits;

 public:
  unsigned IsToRedraw : 1;
  int Eva_Width;
  int Tab_Height;

 private:
  void Set_Active(int select);

  static void const* TabShape;
};
