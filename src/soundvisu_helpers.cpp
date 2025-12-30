#include "legacy/sounddlg.h"
#include "legacy/visudlg.h"

#include <deque>
#include <string>
#include <vector>

#include "legacy/conquer.h"
#include "legacy/checkbox.h"
#include "legacy/externs.h"
#include "legacy/function.h"
#include "legacy/list.h"
#include "legacy/slider.h"
#include "legacy/textbtn.h"
#include "legacy/theme.h"
#include "legacy/toggle.h"

namespace {

int Ui_Factor() { return (SeenBuff.Get_Width() == 320) ? 1 : 2; }

}  // namespace

int SoundControlsClass::Init(void) { return 1; }

void SoundControlsClass::Process(void) {
  const int factor = Ui_Factor();

  const int dialog_w = OPTION_WIDTH * factor;
  const int dialog_h = OPTION_HEIGHT * factor;
  const int dialog_x = (SeenBuff.Get_Width() - dialog_w) / 2;
  const int dialog_y = (SeenBuff.Get_Height() - dialog_h) / 2;
  const int dialog_cx = dialog_x + (dialog_w / 2);

  const int list_x = dialog_x + (LISTBOX_X * factor);
  const int list_y = dialog_y + (LISTBOX_Y * factor);
  const int list_w = LISTBOX_W * factor;
  const int list_h = LISTBOX_H * factor;

  const int stop_x = dialog_x + (STOP_X * factor);
  const int stop_y = dialog_y + (STOP_Y * factor);
  const int play_x = dialog_x + (PLAY_X * factor);
  const int play_y = dialog_y + (PLAY_Y * factor);
  const int shuffle_x = dialog_x + (SHUFFLE_X * factor);
  const int shuffle_y = dialog_y + (SHUFFLE_Y * factor);
  const int repeat_x = dialog_x + (REPEAT_X * factor);
  const int repeat_y = dialog_y + (REPEAT_Y * factor);

  const int music_slider_x = dialog_x + (MSLIDER_X * factor);
  const int music_slider_y = dialog_y + (MSLIDER_Y * factor);
  const int music_slider_w = MSLIDER_W * factor;
  const int music_slider_h = MSLIDER_HEIGHT * factor;

  const int fx_slider_x = dialog_x + (FXSLIDER_X * factor);
  const int fx_slider_y = dialog_y + (FXSLIDER_Y * factor);
  const int fx_slider_w = FXSLIDER_W * factor;
  const int fx_slider_h = FXSLIDER_HEIGHT * factor;

  const int options_w = BUTTON_WIDTH * factor;
  const int options_h = 9 * factor;
  const int options_x = dialog_x + (BUTTON_X * factor);
  const int options_y = dialog_y + (BUTTON_Y * factor);

  void const* up_button = nullptr;
  void const* down_button = nullptr;
  if (InMainLoop) {
    up_button = Hires_Retrieve("BTN-UP.SHP");
    down_button = Hires_Retrieve("BTN-DN.SHP");
  } else {
    up_button = Hires_Retrieve("BTN-UP2.SHP");
    down_button = Hires_Retrieve("BTN-DN2.SHP");
  }

  ListClass track_list(BUTTON_LISTBOX, list_x, list_y, list_w, list_h,
                       TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW, up_button, down_button);

  TextButtonClass stop_btn(BUTTON_STOP, TXT_STOP,
                           TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW, stop_x,
                           stop_y, 16 * factor, 9 * factor);
  TextButtonClass play_btn(BUTTON_PLAY, TXT_PLAY,
                           TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW, play_x,
                           play_y, 16 * factor, 9 * factor);

  CheckBoxClass shuffle_btn(BUTTON_SHUFFLE, shuffle_x, shuffle_y);
  shuffle_btn.IsToggleType = true;
  if (Options.IsScoreShuffle) {
    shuffle_btn.IsOn = true;
  }

  CheckBoxClass repeat_btn(BUTTON_REPEAT, repeat_x, repeat_y);
  repeat_btn.IsToggleType = true;
  if (Options.IsScoreRepeat) {
    repeat_btn.IsOn = true;
  }

  SliderClass music_slider(SLIDER_MUSIC, music_slider_x, music_slider_y, music_slider_w,
                           music_slider_h);
  music_slider.Set_Maximum(255);
  music_slider.Set_Value(Options.ScoreVolume);

  SliderClass fx_slider(SLIDER_SOUND, fx_slider_x, fx_slider_y, fx_slider_w, fx_slider_h);
  fx_slider.Set_Maximum(255);
  fx_slider.Set_Value(Options.Volume);

  TextButtonClass options_btn(BUTTON_OPTIONS, TXT_OPTIONS_MENU,
                              TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW,
                              options_x, options_y, options_w, options_h);

  GadgetClass* commands = &options_btn;
  track_list.Add_Tail(*commands);
  stop_btn.Add_Tail(*commands);
  play_btn.Add_Tail(*commands);
  shuffle_btn.Add_Tail(*commands);
  repeat_btn.Add_Tail(*commands);
  music_slider.Add_Tail(*commands);
  fx_slider.Add_Tail(*commands);

  std::deque<std::string> track_names;
  std::vector<ThemeType> track_ids;
  for (int i = 0; i < Theme.Max_Themes(); ++i) {
    const auto theme = static_cast<ThemeType>(i);
    if (theme == THEME_NONE) continue;
    if (!Theme.Is_Allowed(theme)) continue;
    const char* name = Theme.Full_Name(theme);
    if (!name || !*name) continue;
    track_names.emplace_back(name);
    track_ids.push_back(theme);
    track_list.Add_Item(track_names.back().c_str());
  }
  if (track_list.Count() > 0) {
    track_list.Set_Selected_Index(0);
  }

  Set_Logic_Page(SeenBuff);

  const bool was_hidden = Get_Mouse_State() != 0;
  Show_Mouse();
  Fancy_Text_Print(TXT_NONE, 0, 0, 0, 0, TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW);

  bool redraw = true;
  int last_music = Options.ScoreVolume;
  int last_fx = Options.Volume;
  int last_shuffle = Options.IsScoreShuffle != 0;
  int last_repeat = Options.IsScoreRepeat != 0;

  while (true) {
    if (redraw) {
      Hide_Mouse();
      Dialog_Box(dialog_x, dialog_y, dialog_w, dialog_h);
      Fancy_Text_Print(TXT_SOUND_CONTROLS, dialog_cx, dialog_y + (5 * factor), CC_GREEN, TBLACK,
                       TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW);

      Fancy_Text_Print("Music", dialog_x + (8 * factor), dialog_y + (22 * factor), CC_GREEN, BLACK,
                       TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW);
      Fancy_Text_Print("Sound", dialog_x + (8 * factor), dialog_y + (34 * factor), CC_GREEN, BLACK,
                       TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW);

      Fancy_Text_Print(TXT_SHUFFLE, shuffle_x + (10 * factor), shuffle_y - (1 * factor), CC_GREEN,
                       BLACK, TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW);
      Fancy_Text_Print(TXT_REPEAT, repeat_x + (10 * factor), repeat_y - (1 * factor), CC_GREEN,
                       BLACK, TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW);

      track_list.Draw_Me(true);
      stop_btn.Draw_Me(true);
      play_btn.Draw_Me(true);
      shuffle_btn.Draw_Me(true);
      repeat_btn.Draw_Me(true);
      music_slider.Draw_Me(true);
      fx_slider.Draw_Me(true);
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
      if (selection == BUTTON_STOP) {
        Theme.Stop();
      }
      if (selection == BUTTON_PLAY) {
        const int idx = track_list.Current_Index();
        if (idx >= 0 && idx < static_cast<int>(track_ids.size())) {
          Theme.Play_Song(track_ids[idx]);
        }
      }
      if (selection == BUTTON_SHUFFLE) {
        Options.Set_Shuffle(shuffle_btn.IsOn ? 1 : 0);
      }
      if (selection == BUTTON_REPEAT) {
        Options.Set_Repeat(repeat_btn.IsOn ? 1 : 0);
      }
    }

    const int music = music_slider.Get_Value();
    if (music != last_music) {
      Options.Set_Score_Volume(music);
      last_music = music;
    }

    const int fx = fx_slider.Get_Value();
    if (fx != last_fx) {
      Options.Set_Sound_Volume(fx, false);
      last_fx = fx;
    }

    const int shuffle = shuffle_btn.IsOn != 0;
    if (shuffle != last_shuffle) {
      Options.Set_Shuffle(shuffle);
      last_shuffle = shuffle;
    }
    const int repeat = repeat_btn.IsOn != 0;
    if (repeat != last_repeat) {
      Options.Set_Repeat(repeat);
      last_repeat = repeat;
    }
  }

  if (was_hidden) {
    Hide_Mouse();
  }
}

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
