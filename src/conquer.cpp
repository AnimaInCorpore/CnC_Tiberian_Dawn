#include "legacy_compat.h"
#include "sdl_platform.h"

#include <SDL.h>

void Call_Back()
{
    bool should_quit = false;
    SDL_Platform_Pump_Events(should_quit);
    HidPage.Blit(SeenBuff);
    SDL_Platform_Present_Indexed8(SeenBuff.Data(), SeenBuff.Get_Width(), SeenBuff.Get_Height(), SeenBuff.Pitch());
    SDL_Platform_Delay(16);
}

bool Main_Loop()
{
    bool should_quit = false;
    SDL_Platform_Pump_Events(should_quit);
    HidPage.Blit(SeenBuff);
    SDL_Platform_Present_Indexed8(SeenBuff.Data(), SeenBuff.Get_Width(), SeenBuff.Get_Height(), SeenBuff.Pitch());
    SDL_Platform_Delay(16);
    return should_quit;
}
