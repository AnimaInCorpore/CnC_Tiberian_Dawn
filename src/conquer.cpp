#include "legacy_compat.h"
#include "sdl_platform.h"

#include <SDL.h>

static void Present_SeenBuff()
{
    SDL_Surface* screen = SDL_Platform_Screen();
    if (!screen) return;

    if (SDL_MUSTLOCK(screen)) {
        if (SDL_LockSurface(screen) != 0) return;
    }

    unsigned char const* src = SeenBuff.Data();
    if (src && screen->pixels) {
        const int w = SeenBuff.Get_Width();
        const int h = SeenBuff.Get_Height();
        const int dst_pitch = screen->pitch;
        const int src_pitch = SeenBuff.Pitch();
        unsigned char* dst = (unsigned char*)screen->pixels;

        const int copy_w = (w < screen->w) ? w : screen->w;
        const int copy_h = (h < screen->h) ? h : screen->h;

        for (int y = 0; y < copy_h; ++y) {
            std::memcpy(dst + y * dst_pitch, src + y * src_pitch, (size_t)copy_w);
        }
    }

    if (SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
    SDL_Platform_Present();
}

void Call_Back()
{
    bool should_quit = false;
    SDL_Platform_Pump_Events(should_quit);
    if (LogicPage) {
        LogicPage->Clear();
    } else {
        SeenBuff.Clear();
    }
    Present_SeenBuff();
    SDL_Platform_Delay(16);
}

bool Main_Loop()
{
    bool should_quit = false;
    SDL_Platform_Pump_Events(should_quit);
    Present_SeenBuff();
    SDL_Platform_Delay(16);
    return should_quit;
}
