/*
**	Simplified main menu for the SDL port.
*/

#include <SDL2/SDL.h>

#include "legacy/externs.h"
#include "legacy/function.h"
#include "legacy/gscreen.h"
#include "legacy/textbtn.h"
#include "platform_input.h"
#include "runtime_sdl.h"

#include <vector>

enum MenuButtonIds {
  BUTTON_START = 1,
  BUTTON_LOAD,
  BUTTON_MULTI,
  BUTTON_INTERNET,
  BUTTON_INTRO,
  BUTTON_EXIT,
};

static void Build_Button_List(std::vector<TextButtonClass*>& buttons, ControlClass*& head) {
  const int button_width = 220;
  const int button_height = 20;
  const int spacing = 10;
  const int start_x = (ScreenWidth - button_width) / 2;
  int y = 140;
  const TextPrintType style =
      static_cast<TextPrintType>(TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW);

  auto emplace_button = [&](unsigned id, int text) {
    buttons.push_back(
        new TextButtonClass(id, text, style, start_x, y, button_width, button_height));
    y += button_height + spacing;
  };

  emplace_button(BUTTON_START, TXT_START_NEW_GAME);
  emplace_button(BUTTON_LOAD, TXT_LOAD_MISSION);
  emplace_button(BUTTON_MULTI, TXT_MULTIPLAYER_GAME);
  emplace_button(BUTTON_INTERNET, TXT_INTERNET);
  emplace_button(BUTTON_INTRO, TXT_INTRO);
  emplace_button(BUTTON_EXIT, TXT_EXIT_GAME);

  if (!buttons.empty()) {
    head = buttons.front();
    for (std::size_t i = 1; i < buttons.size(); ++i) {
      buttons[i]->Add_Tail(*head);
    }
  }
}

static void Destroy_Buttons(std::vector<TextButtonClass*>& buttons) {
  for (auto* button : buttons) {
    delete button;
  }
  buttons.clear();
}

int Main_Menu(unsigned long timeout) {
  std::vector<TextButtonClass*> buttons;
  ControlClass* commands = nullptr;
  Build_Button_List(buttons, commands);

  if (buttons.empty() || commands == nullptr) {
    return -1;
  }

  buttons.front()->Turn_On();

  Set_Logic_Page(SeenBuff);
  Keyboard::Clear();
  Show_Mouse();

  // Draw the background/title.
  Load_Title_Screen(const_cast<char*>("HTITLE.PCX"), &HidPage, Palette);
  HidPage.Blit(SeenBuff);

  const unsigned long start_time = TickCount.Time();
  bool running = true;
  int retval = -1;

  while (running && !ReadyToQuit) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT ||
          (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)) {
        ReadyToQuit = true;
        retval = -1;
        running = false;
        break;
      }
      Platform_Handle_Sdl_Event(event);
    }

    if (!running || ReadyToQuit) {
      break;
    }

    if (timeout && TickCount.Time() - start_time > timeout) {
      retval = -1;
      break;
    }

    KeyNumType input = commands->Input();
    switch (input) {
      case (BUTTON_START | KN_BUTTON):
        retval = 0;
        running = false;
        break;
      case (BUTTON_LOAD | KN_BUTTON):
        retval = 1;
        running = false;
        break;
      case (BUTTON_MULTI | KN_BUTTON):
        retval = 2;
        running = false;
        break;
      case (BUTTON_INTERNET | KN_BUTTON):
        retval = 3;
        running = false;
        break;
      case (BUTTON_INTRO | KN_BUTTON):
        retval = 4;
        running = false;
        break;
      case (BUTTON_EXIT | KN_BUTTON):
        retval = 5;
        running = false;
        break;
      default:
        break;
    }

    GScreenClass::Blit_Display();
    SDL_Delay(16);
  }

  Destroy_Buttons(buttons);
  return retval;
}
