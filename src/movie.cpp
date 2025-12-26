// Movie playback (VQA) is part of the original Win95 runtime.
// This port currently provides a minimal, deterministic replacement that:
// - clears the screen if requested
// - pumps input/events and allows early exit
// - keeps menu/game timing comparable (non-blocking frame loop)
//
// Full VQA decode/playback (video + palette + audio) must be implemented to reach parity.

#include <SDL.h>

#include "legacy/debug.h"
#include "legacy/function.h"
#include "legacy/gscreen.h"

#include <chrono>
#include <string>

namespace {

bool Movie_Should_Skip() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) return true;
    if (e.type == SDL_KEYDOWN || e.type == SDL_MOUSEBUTTONDOWN) return true;
  }
  return false;
}

}  // namespace

void Play_Movie(char const* name, ThemeType /*theme*/, bool clrscrn) {
  std::string dbg = "Play_Movie: requested '";
  dbg += (name ? name : "(null)");
  dbg += "'.VQA (decoder pending)\n";
  CCDebugString(dbg.c_str());

  if (clrscrn) {
    VisiblePage.Clear();
    GScreenClass::Blit_Display();
  }

  // Keep behavior deterministic and skippable: run a short frame loop rather than a single delay.
  using clock = std::chrono::steady_clock;
  const auto start = clock::now();
  constexpr int kSimulatedMs = 2000;
  constexpr int kFrameMs = 1000 / 60;

  while (true) {
    if (Movie_Should_Skip()) break;
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - start).count();
    if (elapsed >= kSimulatedMs) break;
    SDL_Delay(kFrameMs);
  }
}

