// Minimal stub implementation of Play_Movie so intro playback can be triggered
// from the menu while the full VQA playback is still being ported.

#include <SDL.h>

#include "legacy/function.h"
#include "legacy/gscreen.h"
#include "legacy/debug.h"

void Play_Movie(char const* name, ThemeType theme, bool clrscrn) {
  std::string dbg = "Play_Movie: requested '";
  dbg += (name ? name : "(null)");
  dbg += "\n";
  CCDebugString(dbg.c_str());

  if (clrscrn) {
    // Clear the visible page to give simple feedback that something happened.
    VisiblePage.Clear();
    GScreenClass::Blit_Display();
  }

  // Simple placeholder: pause for 2 seconds to simulate a movie running.
  SDL_Delay(2000);
}
