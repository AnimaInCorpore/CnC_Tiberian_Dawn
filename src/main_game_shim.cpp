// Minimal SDL-powered Main_Game to provide a runnable, interactive harness
// while the legacy CONQUER.CPP startup and UI stack are being ported.

#include "sdl_platform.h"

void Main_Game(int argc, char* argv[])
{
    (void)argc; (void)argv;
    if (!SDL_Platform_Init(640, 400, false)) {
        return;
    }

    bool should_quit = false;
    while (!should_quit) {
        SDL_Platform_Pump_Events(should_quit);
        SDL_Platform_Clear(0, 0, 0);
        SDL_Platform_Present();
        SDL_Platform_Delay(16);
    }

    SDL_Platform_Shutdown();
}
