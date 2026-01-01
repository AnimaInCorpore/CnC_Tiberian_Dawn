#include <SDL.h>

#include <cstring>

#include "legacy/ccdde.h"
#include "legacy/externs.h"
#include "legacy/function.h"
#include "legacy/gscreen.h"
#include "legacy/textbtn.h"
#include "platform_input.h"

extern bool TD_MenuFadeIn;

namespace {

int Find_Button_Index(TextButtonClass* const* buttons, int count, int button_id) {
  if (!buttons) return -1;
  for (int index = 0; index < count; ++index) {
    if (buttons[index] && buttons[index]->ID == button_id) {
      return index;
    }
  }
  return -1;
}

}  // namespace

int Main_Menu(unsigned long timeout) {
  enum {
    D_DIALOG_W = 152 * 2,
    D_DIALOG_H = 136 * 2,
    D_DIALOG_X = 85 * 2,
    D_DIALOG_Y = 0,
    D_DIALOG_CX = D_DIALOG_X + (D_DIALOG_W / 2),

    D_START_W = 125 * 2,
    D_START_H = 9 * 2,
    D_START_X = 98 * 2,
    D_START_Y = 35 * 2,

#ifdef BONUS_MISSIONS
    D_BONUS_W = 125 * 2,
    D_BONUS_H = 9 * 2,
    D_BONUS_X = 98 * 2,
    D_BONUS_Y = 0,
#endif  // BONUS_MISSIONS

    D_INTERNET_W = 125 * 2,
    D_INTERNET_H = 9 * 2,
    D_INTERNET_X = 98 * 2,
    D_INTERNET_Y = 36 * 2,

    D_LOAD_W = 125 * 2,
    D_LOAD_H = 9 * 2,
    D_LOAD_X = 98 * 2,
    D_LOAD_Y = 53 * 2,

    D_MULTI_W = 125 * 2,
    D_MULTI_H = 9 * 2,
    D_MULTI_X = 98 * 2,
    D_MULTI_Y = 71 * 2,

    D_INTRO_W = 125 * 2,
    D_INTRO_H = 9 * 2,
    D_INTRO_X = 98 * 2,
    D_INTRO_Y = 89 * 2,
#if (GERMAN | FRENCH)
    D_EXIT_W = 83 * 2,
#else
    D_EXIT_W = 63 * 2,
#endif
    D_EXIT_H = 9 * 2,
#if (GERMAN | FRENCH)
    D_EXIT_X = 118 * 2,
#else
    D_EXIT_X = 128 * 2,
#endif
    D_EXIT_Y = 111 * 2,

  };

#ifdef NEWMENU
  int starty = 25 * 2;
#endif

  enum {
#ifdef NEWMENU
    BUTTON_EXPAND = 100 * 2,
    BUTTON_START,
#ifdef BONUS_MISSIONS
    BUTTON_BONUS,
#endif  // BONUS_MISSIONS
    BUTTON_INTERNET,
#else
    BUTTON_START = 100 * 2,
#endif
    BUTTON_LOAD,
    BUTTON_MULTI,
    BUTTON_INTRO,
    BUTTON_EXIT,
  };

#ifdef NEWMENU
  const bool expansions = Expansion_Present();
#endif
  KeyNumType input;
  int retval = -1;
  int curbutton;
#ifdef NEWMENU
#ifdef BONUS_MISSIONS
  TextButtonClass* buttons[8];
#else
  TextButtonClass* buttons[7];
#endif  // BONUS_MISSIONS
#else
  TextButtonClass* buttons[5];
#endif
  unsigned long starttime;

  ControlClass* commands = nullptr;

#ifdef NEWMENU
#ifdef BONUS_MISSIONS
  int ystep = 13 * 2;
#else
  int ystep = 15 * 2;
#endif  // BONUS_MISSIONS

  if (expansions) {
    ystep -= 2 * 2;
  }
  TextButtonClass expandbtn(BUTTON_EXPAND, TXT_NEW_MISSIONS,
                            static_cast<TextPrintType>(TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL |
                                                       TPF_NOSHADOW),
                            D_START_X, starty, D_START_W, D_START_H);
  if (expansions) {
    starty += ystep;
  }

  TextButtonClass startbtn(BUTTON_START, TXT_START_NEW_GAME,
                           static_cast<TextPrintType>(TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL |
                                                      TPF_NOSHADOW),
                           D_START_X, starty, D_START_W, D_START_H);
  starty += ystep;

#ifdef BONUS_MISSIONS
  TextButtonClass bonusbtn(BUTTON_BONUS, TXT_BONUS_MISSIONS,
                           static_cast<TextPrintType>(TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL |
                                                      TPF_NOSHADOW),
                           D_BONUS_X, starty, D_BONUS_W, D_BONUS_H);
  starty += ystep;
#endif  // BONUS_MISSIONS

  TextButtonClass internetbutton(BUTTON_INTERNET, TXT_INTERNET,
                                 static_cast<TextPrintType>(TPF_CENTER | TPF_6PT_GRAD |
                                                            TPF_USE_GRAD_PAL | TPF_NOSHADOW),
                                 D_INTERNET_X, starty, D_INTERNET_W, D_INTERNET_H);
  starty += ystep;

  TextButtonClass loadbtn(BUTTON_LOAD, TXT_LOAD_MISSION,
                          static_cast<TextPrintType>(TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL |
                                                     TPF_NOSHADOW),
                          D_LOAD_X, starty, D_LOAD_W, D_LOAD_H);
  starty += ystep;
#else

  TextButtonClass startbtn(BUTTON_START, TXT_START_NEW_GAME,
                           static_cast<TextPrintType>(TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL |
                                                      TPF_NOSHADOW),
                           D_START_X, D_START_Y, D_START_W, D_START_H);

  TextButtonClass loadbtn(BUTTON_LOAD, TXT_LOAD_MISSION,
                          static_cast<TextPrintType>(TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL |
                                                     TPF_NOSHADOW),
                          D_LOAD_X, D_LOAD_Y, D_LOAD_W, D_LOAD_H);

#endif

#ifdef DEMO
  TextButtonClass multibtn(BUTTON_MULTI, TXT_ORDER_INFO,
                           static_cast<TextPrintType>(TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL |
                                                      TPF_NOSHADOW),
                           D_MULTI_X, D_MULTI_Y, D_MULTI_W, D_MULTI_H);
#else

#ifdef NEWMENU
  TextButtonClass multibtn(BUTTON_MULTI, TXT_MULTIPLAYER_GAME,
                           static_cast<TextPrintType>(TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL |
                                                      TPF_NOSHADOW),
                           D_MULTI_X, starty, D_MULTI_W, D_MULTI_H);
  starty += ystep;

#else
  TextButtonClass multibtn(BUTTON_MULTI, TXT_MULTIPLAYER_GAME,
                           static_cast<TextPrintType>(TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL |
                                                      TPF_NOSHADOW),
                           D_MULTI_X, D_MULTI_Y, D_MULTI_W, D_MULTI_H);
#endif
#endif

#ifdef NEWMENU
#ifdef DEMO
  TextButtonClass introbtn(BUTTON_INTRO, TXT_JUST_INTRO,
#else   // DEMO
  TextButtonClass introbtn(BUTTON_INTRO, TXT_INTRO,
#endif  // DEMO
                           static_cast<TextPrintType>(TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL |
                                                      TPF_NOSHADOW),
                           D_INTRO_X, starty, D_INTRO_W, D_INTRO_H);
  starty += ystep;

  TextButtonClass exitbtn(BUTTON_EXIT, TXT_EXIT_GAME,
                          static_cast<TextPrintType>(TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL |
                                                     TPF_NOSHADOW),
#if (GERMAN | FRENCH)
                          D_EXIT_X, starty, D_EXIT_W, D_EXIT_H);
#else
                          D_EXIT_X, starty, D_EXIT_W, D_EXIT_H);
#endif
  starty += ystep;

#else

#ifdef DEMO
  TextButtonClass introbtn(BUTTON_INTRO, TXT_JUST_INTRO,
#else   // DEMO
  TextButtonClass introbtn(BUTTON_INTRO, TXT_INTRO,
#endif  // DEMO
                           static_cast<TextPrintType>(TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL |
                                                      TPF_NOSHADOW),
                           D_INTRO_X, D_INTRO_Y, D_INTRO_W, D_INTRO_H);

  TextButtonClass exitbtn(BUTTON_EXIT, TXT_EXIT_GAME,
                          static_cast<TextPrintType>(TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL |
                                                     TPF_NOSHADOW),
#if (GERMAN | FRENCH)
                          D_EXIT_X, D_EXIT_Y, D_EXIT_W, D_EXIT_H);
#else
                          D_EXIT_X, D_EXIT_Y, D_EXIT_W, D_EXIT_H);
#endif
#endif

  Set_Logic_Page(SeenBuff);
  Keyboard::Clear();
  starttime = TickCount.Time();

  commands = &startbtn;
#ifdef NEWMENU
  if (expansions) {
    expandbtn.Add_Tail(*commands);
  }
#endif
#ifdef BONUS_MISSIONS
  bonusbtn.Add_Tail(*commands);
#endif  // BONUS_MISSIONS

#ifndef DEMO
  internetbutton.Add_Tail(*commands);
#endif  // DEMO
  loadbtn.Add_Tail(*commands);
  multibtn.Add_Tail(*commands);
  introbtn.Add_Tail(*commands);
  exitbtn.Add_Tail(*commands);

#ifdef NEWMENU
  if (expansions) {
    curbutton = 0;
  } else {
    curbutton = 1;
  }
  int butt = 0;

  buttons[butt++] = &expandbtn;
  buttons[butt++] = &startbtn;
#ifdef BONUS_MISSIONS
  buttons[butt++] = &bonusbtn;
#endif  // BONUS_MISSIONS
  buttons[butt++] = &internetbutton;
  buttons[butt++] = &loadbtn;
  buttons[butt++] = &multibtn;
  buttons[butt++] = &introbtn;
  buttons[butt++] = &exitbtn;
#else
  curbutton = 0;
  buttons[0] = &startbtn;
  buttons[1] = &loadbtn;
  buttons[2] = &multibtn;
  buttons[3] = &introbtn;
  buttons[4] = &exitbtn;
#endif

  // Win95 pre-selects the multiplayer entry when returning from a multiplayer game.
  if (GameToPlay != GAME_NORMAL) {
    const int multi_index = Find_Button_Index(buttons, static_cast<int>(sizeof(buttons) / sizeof(buttons[0])),
                                              BUTTON_MULTI);
    if (multi_index >= 0) {
      curbutton = multi_index;
    }
  }
  buttons[curbutton]->Turn_On();

  Keyboard::Clear();

  Fancy_Text_Print(TXT_NONE, 0, 0, CC_GREEN, TBLACK,
                   static_cast<TextPrintType>(TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL |
                                              TPF_NOSHADOW));
  while (Get_Mouse_State() > 0) {
    Show_Mouse();
  }

  bool display = true;
  bool process = true;
  while (process && !ReadyToQuit) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT ||
          (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)) {
        ReadyToQuit = true;
        retval = -1;
        process = false;
        break;
      }
      if (event.type == SDL_WINDOWEVENT) {
        if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
          GameInFocus = true;
        } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
          GameInFocus = false;
        }
      }
      Platform_Handle_Sdl_Event(event);
    }

    if (!process || ReadyToQuit) {
      break;
    }

    if (AllSurfaces.SurfacesRestored) {
      AllSurfaces.SurfacesRestored = FALSE;
      display = true;
    }

    if (timeout && TickCount.Time() - starttime > timeout) {
      retval = -1;
      break;
    }

    Call_Back();

    if (display) {
      Load_Title_Screen(const_cast<char*>("HTITLE.PCX"), &HidPage, Palette);
      HidPage.Blit(SeenBuff);
      if (GamePalette && Palette) {
        std::memcpy(GamePalette, Palette, 768);
      }
      if (TD_MenuFadeIn && Palette) {
        Fade_Palette_To(Palette, FADE_PALETTE_SLOW, Call_Back);
        TD_MenuFadeIn = false;
      }

      Set_Logic_Page(HidPage);
      Dialog_Box(D_DIALOG_X, D_DIALOG_Y, D_DIALOG_W, D_DIALOG_H);
      Draw_Caption(TXT_NONE, D_DIALOG_X, D_DIALOG_Y, D_DIALOG_W);
#ifdef VIRGIN_CHEAT_KEYS
#ifdef DEMO
      Version_Number();
      Fancy_Text_Print("Demo%s", D_DIALOG_X + D_DIALOG_W - 5 * 2, D_DIALOG_Y + D_DIALOG_H - 10 * 2,
                       DKGREY, TBLACK,
                       static_cast<TextPrintType>(TPF_6POINT | TPF_FULLSHADOW | TPF_RIGHT),
                       VersionText);
#else
      Fancy_Text_Print("V.%d%s", D_DIALOG_X + D_DIALOG_W - 5 * 2, D_DIALOG_Y + D_DIALOG_H - 10 * 2,
                       DKGREY, TBLACK,
                       static_cast<TextPrintType>(TPF_6POINT | TPF_FULLSHADOW | TPF_RIGHT),
                       Version_Number(), VersionText, FOREIGN_VERSION_NUMBER);
#endif
#else
#ifdef DEMO
      Version_Number();
      Fancy_Text_Print("Demo%s", D_DIALOG_X + D_DIALOG_W - 5 * 2, D_DIALOG_Y + D_DIALOG_H - 10 * 2,
                       DKGREY, TBLACK,
                       static_cast<TextPrintType>(TPF_6POINT | TPF_FULLSHADOW | TPF_RIGHT),
                       VersionText);
#else
      Fancy_Text_Print("V.%d%s", D_DIALOG_X + D_DIALOG_W - 5 * 2, D_DIALOG_Y + D_DIALOG_H - 10 * 2,
                       DKGREY, TBLACK,
                       static_cast<TextPrintType>(TPF_6POINT | TPF_FULLSHADOW | TPF_RIGHT),
                       Version_Number(), VersionText);
#endif
#endif

      Hide_Mouse();
      HidPage.Blit(SeenBuff);
      Show_Mouse();

      Set_Logic_Page(SeenBuff);
      startbtn.Draw_All();
      if (ScreenWidth == 320) {
        ModeX_Blit(SeenBuff.Get_Graphic_Buffer());
      }
      display = false;
    }

#ifndef DEMO
    if (DDEServer.Get_MPlayer_Game_Info()) {
      retval = BUTTON_INTERNET - BUTTON_EXPAND;
      process = false;
    }
#endif  // DEMO

    input = commands->Input();
    switch (input) {
#ifdef NEWMENU
      case (BUTTON_EXPAND | KN_BUTTON):
        retval = (input & 0x7FFF) - BUTTON_EXPAND;
        process = false;
        break;

      case (BUTTON_INTERNET | KN_BUTTON):
        retval = (input & 0x7FFF) - BUTTON_EXPAND;
        process = false;
        break;

#else
#define BUTTON_EXPAND BUTTON_START
#endif

      case (BUTTON_START | KN_BUTTON):
        retval = (input & 0x7FFF) - BUTTON_EXPAND;
        process = false;
        break;

#ifdef BONUS_MISSIONS
      case (BUTTON_BONUS | KN_BUTTON):
        retval = (input & 0x7FFF) - BUTTON_EXPAND;
        process = false;
        break;
#endif  // BONUS_MISSIONS

      case (BUTTON_LOAD | KN_BUTTON):
        retval = (input & 0x7FFF) - BUTTON_EXPAND;
#ifdef DEMO
        retval += 1;
#endif  // DEMO
        process = false;
        break;

      case (BUTTON_MULTI | KN_BUTTON):
        retval = (input & 0x7FFF) - BUTTON_EXPAND;
#ifdef DEMO
        retval += 1;
#endif  // DEMO
        process = false;
        break;

      case (BUTTON_INTRO | KN_BUTTON):
        retval = (input & 0x7FFF) - BUTTON_EXPAND;
#ifdef DEMO
        retval += 1;
#endif  // DEMO
        process = false;
        break;

      case (BUTTON_EXIT | KN_BUTTON):
        retval = (input & 0x7FFF) - BUTTON_EXPAND;
#ifdef DEMO
        retval += 1;
#endif  // DEMO
        process = false;
        break;

      case KN_UP:
        buttons[curbutton]->Turn_Off();
        buttons[curbutton]->Flag_To_Redraw();
        curbutton--;
#ifdef NEWMENU
        if (expansions) {
          if (curbutton < 0) {
            curbutton = 6;
          }
        } else {
          if (curbutton < 1) {
            curbutton = 6;
          }
        }
#else
        if (curbutton < 0) {
          curbutton = 4;
        }
#endif
        buttons[curbutton]->Turn_On();
        buttons[curbutton]->Flag_To_Redraw();
        break;

      case KN_DOWN:
        buttons[curbutton]->Turn_Off();
        buttons[curbutton]->Flag_To_Redraw();
        curbutton++;
#ifdef NEWMENU
        if (curbutton > 6) {
          if (expansions) {
            curbutton = 0;
          } else {
            curbutton = 1;
          }
        }
#else
        if (curbutton > 4) {
          curbutton = 0;
        }
#endif
        buttons[curbutton]->Turn_On();
        buttons[curbutton]->Flag_To_Redraw();
        break;

      case KN_RETURN:
        buttons[curbutton]->IsPressed = true;
        buttons[curbutton]->Draw_Me(true);
        retval = curbutton;
        process = false;
        break;

      default:
        break;
    }

    GScreenClass::Blit_Display();
    SDL_Delay(16);
  }
  return retval;
}
