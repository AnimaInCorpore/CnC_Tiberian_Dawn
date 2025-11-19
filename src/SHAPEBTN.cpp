#include "function.h"
#include "shapebtn.h"

ShapeButtonClass::ShapeButtonClass() : ToggleClass(0, 0, 0, 0, 0) { ReflectButtonState = false; }

ShapeButtonClass::ShapeButtonClass(unsigned id, void const* shape, int x, int y)
    : ToggleClass(id, x, y, 0, 0) {
  ReflectButtonState = false;
  Width = 0;
  Height = 0;
  Set_Shape(shape);
}

void ShapeButtonClass::Set_Shape(void const* data) {
  ShapeData = data;
  if (ShapeData) {
    Width = Get_Build_Frame_Width(ShapeData);
    Height = Get_Build_Frame_Height(ShapeData);
  }
}

int ShapeButtonClass::Draw_Me(int forced) {
  if (!ControlClass::Draw_Me(forced) || !ShapeData) {
    return false;
  }

  if (LogicPage == &SeenBuff) {
    Conditional_Hide_Mouse(X, Y, X + Width - 1, Y + Height - 1);
  }

  int frame = 0;
  if (IsDisabled) {
    frame = DISABLED_SHAPE;
  } else if (ReflectButtonState) {
    frame = IsOn ? DOWN_SHAPE : UP_SHAPE;
  } else if (IsPressed) {
    frame = DOWN_SHAPE;
  }

  CC_Draw_Shape(ShapeData, frame, X, Y, WINDOW_MAIN, SHAPE_NORMAL);

  if (LogicPage == &SeenBuff) {
    Conditional_Show_Mouse();
  }

  return true;
}
