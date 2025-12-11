#include "function.h"

#include <algorithm>

namespace {
int g_mouse_hide_depth = 0;
bool g_mouse_conditionally_hidden = false;

bool Is_Mouse_Within(int left, int top, int right, int bottom) {
  if (left > right) {
    std::swap(left, right);
  }
  if (top > bottom) {
    std::swap(top, bottom);
  }
  const int mouse_x = Get_Mouse_X();
  const int mouse_y = Get_Mouse_Y();
  return mouse_x >= left && mouse_x <= right && mouse_y >= top && mouse_y <= bottom;
}

GraphicViewPortClass* Active_Mouse_Page() {
  if (LogicPage) {
    return LogicPage;
  }
  return &HidPage;
}

void Erase_Mouse() {
  if (WWMouse) {
    WWMouse->Erase_Mouse(Active_Mouse_Page(), true);
  }
}

void Draw_Mouse() {
  if (WWMouse) {
    WWMouse->Draw_Mouse(Active_Mouse_Page());
  }
}
}  // namespace

void Hide_Mouse() {
  if (g_mouse_hide_depth == 0) {
    Erase_Mouse();
  }
  ++g_mouse_hide_depth;
}

void Show_Mouse() {
  if (g_mouse_hide_depth == 0) {
    return;
  }
  --g_mouse_hide_depth;
  if (g_mouse_hide_depth == 0) {
    Draw_Mouse();
  }
}

void Conditional_Hide_Mouse(int left, int top, int right, int bottom) {
  if (!Is_Mouse_Within(left, top, right, bottom)) {
    return;
  }
  Hide_Mouse();
  g_mouse_conditionally_hidden = true;
}

void Conditional_Show_Mouse() {
  if (!g_mouse_conditionally_hidden) {
    return;
  }
  g_mouse_conditionally_hidden = false;
  Show_Mouse();
}

int Get_Mouse_State(void) { return g_mouse_hide_depth; }
