#include "legacy/function.h"
#include "legacy/gamedlg.h"
#include "legacy/sounddlg.h"
#include "legacy/visudlg.h"

void GameControlsClass::Process(void) {
  int factor = (SeenBuff.Get_Width() == 320) ? 1 : 2;

  int d_dialog_w = 232 * factor;
  int d_dialog_h = 141 * factor;
  int d_dialog_x = ((SeenBuff.Get_Width() - d_dialog_w) / 2);
  int d_dialog_y = ((SeenBuff.Get_Height() - d_dialog_h) / 2);
  int d_dialog_cx = d_dialog_x + (d_dialog_w / 2);
  int d_top_margin = 30 * factor;

  int d_txt6_h = 7 * factor;
  int d_margin1 = 5 * factor;
  int d_margin2 = 2 * factor;

  int d_speed_w = d_dialog_w - (20 * factor);
  int d_speed_h = 6 * factor;
  int d_speed_x = d_dialog_x + (10 * factor);
  int d_speed_y = d_dialog_y + d_top_margin + d_margin1 + d_txt6_h;

  int d_scroll_w = d_dialog_w - (20 * factor);
  int d_scroll_h = 6 * factor;
  int d_scroll_x = d_dialog_x + (10 * factor);
  int d_scroll_y = d_speed_y + d_speed_h + d_txt6_h + (d_margin1 * 2) + d_txt6_h;

  int d_visual_w = d_dialog_w - (40 * factor);
  int d_visual_h = 9 * factor;
  int d_visual_x = d_dialog_x + (20 * factor);
  int d_visual_y = d_scroll_y + d_scroll_h + d_txt6_h + (d_margin1 * 2);

  int d_sound_w = d_dialog_w - (40 * factor);
  int d_sound_h = 9 * factor;
  int d_sound_x = d_dialog_x + (20 * factor);
  int d_sound_y = d_visual_y + d_visual_h + d_margin1;

  int d_ok_w = 20 * factor;
  int d_ok_h = 9 * factor;
  int d_ok_x = d_dialog_cx - (d_ok_w / 2);
  int d_ok_y = d_dialog_y + d_dialog_h - d_ok_h - d_margin1;

  enum {
    BUTTON_SPEED = 100,
    BUTTON_SCROLLRATE,
    BUTTON_VISUAL,
    BUTTON_SOUND,
    BUTTON_OK,
    BUTTON_COUNT,
    BUTTON_FIRST = BUTTON_SPEED,
  };

  KeyNumType input;

  int gamespeed = Options.GameSpeed;
  int scrollrate = Options.ScrollRate;
  int selection;

  GadgetClass* commands;

  SliderClass gspeed_btn(BUTTON_SPEED, d_speed_x, d_speed_y, d_speed_w, d_speed_h);

  SliderClass scrate_btn(BUTTON_SCROLLRATE, d_scroll_x, d_scroll_y, d_scroll_w, d_scroll_h);

  TextButtonClass visual_btn(BUTTON_VISUAL, TXT_VISUAL_CONTROLS,
                             TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW,
                             d_visual_x, d_visual_y, d_visual_w, d_visual_h);

  TextButtonClass sound_btn(BUTTON_SOUND, TXT_SOUND_CONTROLS,
                            TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW,
                            d_sound_x, d_sound_y, d_sound_w, d_sound_h);

  TextButtonClass okbtn(BUTTON_OK, TXT_OPTIONS_MENU,
                        TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW, d_ok_x,
                        d_ok_y);
  okbtn.X = (SeenBuff.Get_Width() - okbtn.Width) / 2;

  Set_Logic_Page(SeenBuff);

  commands = &okbtn;
  gspeed_btn.Add_Tail(*commands);
  scrate_btn.Add_Tail(*commands);
  visual_btn.Add_Tail(*commands);
  sound_btn.Add_Tail(*commands);

  gspeed_btn.Set_Maximum(Options.Normalize_Delay(60));
  gspeed_btn.Set_Value(gamespeed);
  scrate_btn.Set_Maximum(Options.Normalize_Delay(60));
  scrate_btn.Set_Value(scrollrate);

  bool hidden = Get_Mouse_State();
  Show_Mouse();
  Fancy_Text_Print(TXT_NONE, 0, 0, 0, 0, TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW);

  bool redraw = true;
  while (true) {
    if (redraw) {
      Hide_Mouse();
      Dialog_Box(d_dialog_x, d_dialog_y, d_dialog_w, d_dialog_h);
      Fancy_Text_Print(TXT_GAME_CONTROLS, d_dialog_cx, d_dialog_y + d_margin1, CC_GREEN, TBLACK,
                       TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW);

      gspeed_btn.Draw_Me(true);
      scrate_btn.Draw_Me(true);

      Fancy_Text_Print("Game Speed", d_speed_x + d_margin2, d_speed_y - d_txt6_h, CC_GREEN,
                       BLACK, TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW);
      Fancy_Text_Print("Scroll Rate", d_scroll_x + d_margin2, d_scroll_y - d_txt6_h, CC_GREEN,
                       BLACK, TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW);

      visual_btn.Draw_Me(true);
      sound_btn.Draw_Me(true);
      okbtn.Draw_Me(true);
      Show_Mouse();
      redraw = false;
    }

    input = Keyboard::Get();
    if (input == KN_ESC) {
      break;
    }

    selection = commands->Input();
    if (selection != 0) {
      if (selection == BUTTON_OK) {
        break;
      } else if (selection == BUTTON_VISUAL) {
        VisualControlsClass().Process();
        redraw = true;
      } else if (selection == BUTTON_SOUND) {
        SoundControlsClass().Process();
        redraw = true;
      }
    }

    int slider = gspeed_btn.Get_Value();
    if (slider != gamespeed) {
      gamespeed = slider;
      Options.GameSpeed = gamespeed;
    }
    slider = scrate_btn.Get_Value();
    if (slider != scrollrate) {
      scrollrate = slider;
      Options.ScrollRate = scrollrate;
    }

    (void)input;
  }

  if (hidden) {
    Hide_Mouse();
  }
}
