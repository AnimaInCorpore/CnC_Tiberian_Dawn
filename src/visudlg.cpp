#include "legacy/visudlg.h"

#include "legacy/conquer.h"
#include "legacy/externs.h"
#include "legacy/function.h"
#include "legacy/slider.h"
#include "legacy/textbtn.h"

namespace {

int Ui_Factor() { return (SeenBuff.Get_Width() == 320) ? 1 : 2; }

}  // namespace

int VisualControlsClass::Init(void) { return 1; }

void VisualControlsClass::Process(void) {
  const int factor = Ui_Factor();

  const int dialog_w = OPTION_WIDTH * factor;
  const int dialog_h = OPTION_HEIGHT * factor;
  const int dialog_x = (SeenBuff.Get_Width() - dialog_w) / 2;
  const int dialog_y = (SeenBuff.Get_Height() - dialog_h) / 2;
  const int dialog_cx = dialog_x + (dialog_w / 2);

  const int slider_w = SLIDER_WIDTH * factor;
  const int slider_h = SLIDER_HEIGHT * factor;
  const int slider_x = (SLIDER_X - OPTION_X) * factor + dialog_x;
  const int slider_y0 = (SLIDER_Y - OPTION_Y) * factor + dialog_y;
  const int text_x = (TEXT_X - OPTION_X) * factor + dialog_x;
  const int text_y0 = (TEXT_Y - OPTION_Y) * factor + dialog_y;
  const int y_spacing = SLIDER_Y_SPACING * factor;

  const int button_w = 90 * factor;
  const int button_h = 9 * factor;
  const int button_x = (BUTTON_X - OPTION_X) * factor + dialog_x;
  const int button_y = (BUTTON_Y - OPTION_Y) * factor + dialog_y;

  enum {
    SLIDER_BRIGHTNESS = 701,
    SLIDER_COLOR,
    SLIDER_CONTRAST,
    SLIDER_TINT,
  };

  SliderClass brightness_slider(SLIDER_BRIGHTNESS, slider_x, slider_y0 + (0 * y_spacing), slider_w,
                                slider_h);
  SliderClass color_slider(SLIDER_COLOR, slider_x, slider_y0 + (1 * y_spacing), slider_w, slider_h);
  SliderClass contrast_slider(SLIDER_CONTRAST, slider_x, slider_y0 + (2 * y_spacing), slider_w,
                              slider_h);
  SliderClass tint_slider(SLIDER_TINT, slider_x, slider_y0 + (3 * y_spacing), slider_w, slider_h);

  brightness_slider.Set_Maximum(255);
  color_slider.Set_Maximum(255);
  contrast_slider.Set_Maximum(255);
  tint_slider.Set_Maximum(255);

  brightness_slider.Set_Value(Options.Get_Brightness());
  color_slider.Set_Value(Options.Get_Color());
  contrast_slider.Set_Value(Options.Get_Contrast());
  tint_slider.Set_Value(Options.Get_Tint());

  TextButtonClass reset_btn(BUTTON_RESET, TXT_RESET_MENU,
                            TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW, dialog_x,
                            button_y, button_w, button_h);
  reset_btn.X = dialog_x + (20 * factor);

  TextButtonClass options_btn(BUTTON_OPTIONS, TXT_OPTIONS_MENU,
                              TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW, button_x,
                              button_y, button_w, button_h);
  options_btn.X = dialog_x + dialog_w - button_w - (20 * factor);

  GadgetClass* commands = &options_btn;
  brightness_slider.Add_Tail(*commands);
  color_slider.Add_Tail(*commands);
  contrast_slider.Add_Tail(*commands);
  tint_slider.Add_Tail(*commands);
  reset_btn.Add_Tail(*commands);

  Set_Logic_Page(SeenBuff);

  const bool was_hidden = Get_Mouse_State() != 0;
  Show_Mouse();
  Fancy_Text_Print(TXT_NONE, 0, 0, 0, 0, TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW);

  bool redraw = true;
  int last_brightness = brightness_slider.Get_Value();
  int last_color = color_slider.Get_Value();
  int last_contrast = contrast_slider.Get_Value();
  int last_tint = tint_slider.Get_Value();

  while (true) {
    if (redraw) {
      Hide_Mouse();
      Dialog_Box(dialog_x, dialog_y, dialog_w, dialog_h);
      Fancy_Text_Print(TXT_VISUAL_CONTROLS, dialog_cx, dialog_y + (5 * factor), CC_GREEN, TBLACK,
                       TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW);

      Fancy_Text_Print("Brightness", text_x, text_y0 + (0 * y_spacing), CC_GREEN, BLACK,
                       TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW);
      Fancy_Text_Print("Color", text_x, text_y0 + (1 * y_spacing), CC_GREEN, BLACK,
                       TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW);
      Fancy_Text_Print("Contrast", text_x, text_y0 + (2 * y_spacing), CC_GREEN, BLACK,
                       TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW);
      Fancy_Text_Print("Tint", text_x, text_y0 + (3 * y_spacing), CC_GREEN, BLACK,
                       TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW);

      brightness_slider.Draw_Me(true);
      color_slider.Draw_Me(true);
      contrast_slider.Draw_Me(true);
      tint_slider.Draw_Me(true);
      reset_btn.Draw_Me(true);
      options_btn.Draw_Me(true);

      Show_Mouse();
      redraw = false;
    }

    const KeyNumType key = Keyboard::Get();
    if (key == KN_ESC) {
      break;
    }

    const int selection = commands->Input();
    if (selection != 0) {
      if (selection == BUTTON_OPTIONS) {
        break;
      }
      if (selection == BUTTON_RESET) {
        brightness_slider.Set_Value(128);
        color_slider.Set_Value(128);
        contrast_slider.Set_Value(128);
        tint_slider.Set_Value(128);
        redraw = true;
      }
    }

    const int brightness = brightness_slider.Get_Value();
    if (brightness != last_brightness) {
      Options.Set_Brightness(brightness);
      last_brightness = brightness;
    }
    const int color = color_slider.Get_Value();
    if (color != last_color) {
      Options.Set_Color(color);
      last_color = color;
    }
    const int contrast = contrast_slider.Get_Value();
    if (contrast != last_contrast) {
      Options.Set_Contrast(contrast);
      last_contrast = contrast;
    }
    const int tint = tint_slider.Get_Value();
    if (tint != last_tint) {
      Options.Set_Tint(tint);
      last_tint = tint;
    }
  }

  if (was_hidden) {
    Hide_Mouse();
  }
}

