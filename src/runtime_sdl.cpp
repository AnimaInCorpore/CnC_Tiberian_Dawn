#include "runtime_sdl.h"

namespace {
SDL_Window* g_window = nullptr;
SDL_Renderer* g_renderer = nullptr;
}  // namespace

void Runtime_Set_Sdl_Window(SDL_Window* window) { g_window = window; }

void Runtime_Set_Sdl_Renderer(SDL_Renderer* renderer) { g_renderer = renderer; }

SDL_Window* Runtime_Get_Sdl_Window() { return g_window; }

SDL_Renderer* Runtime_Get_Sdl_Renderer() { return g_renderer; }
