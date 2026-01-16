#include "sdl_platform.h"

#include <SDL.h>

static SDL_Surface* g_screen = 0;

bool SDL_Platform_Init(int width, int height, bool fullscreen)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        return false;
    }

    const int bpp = 32;
    unsigned int flags = SDL_SWSURFACE;
    if (fullscreen) {
        flags |= SDL_FULLSCREEN;
    }

    g_screen = SDL_SetVideoMode(width, height, bpp, flags);
    if (!g_screen) {
        SDL_Quit();
        return false;
    }

    SDL_WM_SetCaption("cnc_td (SDL 1.2)", 0);
    return true;
}

void SDL_Platform_Shutdown()
{
    g_screen = 0;
    SDL_Quit();
}

bool SDL_Platform_Pump_Events(bool& should_quit)
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            should_quit = true;
        }
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
            should_quit = true;
        }
    }
    return true;
}

void SDL_Platform_Clear(unsigned char r, unsigned char g, unsigned char b)
{
    if (!g_screen) return;
    unsigned int color = SDL_MapRGB(g_screen->format, r, g, b);
    SDL_FillRect(g_screen, 0, color);
}

void SDL_Platform_Present()
{
    if (!g_screen) return;
    SDL_Flip(g_screen);
}

unsigned long SDL_Platform_Ticks()
{
    return SDL_GetTicks();
}

void SDL_Platform_Delay(unsigned long ms)
{
    SDL_Delay(ms);
}
