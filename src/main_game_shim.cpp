// Minimal SDL-powered Main_Game to provide a runnable, interactive harness
// while the legacy CONQUER.CPP startup and UI stack are being ported.

#include "sdl_platform.h"
#include "confdlg.h"
#include "legacy_compat.h"
#include "menus.h"

#include <SDL.h>

void Main_Game(int argc, char* argv[])
{
    (void)argc; (void)argv;
    if (!SDL_Platform_Init(640, 400, false)) {
        return;
    }

    SDL_Surface* screen = SDL_Platform_Screen();
    if (screen) {
        SDL_Color pal[256];
        for (int i = 0; i < 256; ++i) {
            pal[i].r = (unsigned char)i;
            pal[i].g = (unsigned char)i;
            pal[i].b = (unsigned char)i;
        }
        pal[0].r = pal[0].g = pal[0].b = 0;
        pal[2].r = 0; pal[2].g = 180; pal[2].b = 0;
        pal[7].r = pal[7].g = pal[7].b = 120;
        pal[15].r = pal[15].g = pal[15].b = 255;
        pal[141].r = 0; pal[141].g = 80; pal[141].b = 0;
        pal[167].r = 0; pal[167].g = 255; pal[167].b = 0;
        SDL_SetPalette(screen, SDL_LOGPAL | SDL_PHYSPAL, pal, 0, 256);
    }

    Set_Logic_Page(SeenBuff);
    for (;;) {
        int choice = Main_Menu(0);
        if (choice < 0 || choice == 3) break;

        ConfirmationClass confirm;
        switch (choice) {
            case 0:
                (void)confirm.Process("New game is not ported yet.");
                break;
            case 1:
                (void)confirm.Process("Load mission is not ported yet.");
                break;
            case 2:
                (void)confirm.Process("Options are not ported yet.");
                break;
            default:
                break;
        }
    }

    SDL_Platform_Shutdown();
}
