// Minimal SDL-powered Main_Game to provide a runnable, interactive harness
// while the legacy CONQUER.CPP startup and UI stack are being ported.

#include "sdl_platform.h"
#include "ccfile.h"
#include "confdlg.h"
#include "legacy_compat.h"
#include "menus.h"

#include <SDL.h>

static bool Load_VGA_Palette(const char* filename, SDL_Surface* screen) {
    if (!screen) return false;

    CCFileClass file(filename);
    if (!file.Is_Available()) return false;

    unsigned char raw[768];
    if (file.Read(raw, 768) != 768) return false;

    SDL_Color pal[256];
    for (int i = 0; i < 256; ++i) {
        unsigned char r6 = raw[i * 3 + 0];
        unsigned char g6 = raw[i * 3 + 1];
        unsigned char b6 = raw[i * 3 + 2];
        pal[i].r = static_cast<unsigned char>(r6 * 4);
        pal[i].g = static_cast<unsigned char>(g6 * 4);
        pal[i].b = static_cast<unsigned char>(b6 * 4);
    }

    SDL_SetPalette(screen, SDL_LOGPAL | SDL_PHYSPAL, pal, 0, 256);
    return true;
}

void Main_Game(int argc, char* argv[])
{
    (void)argc; (void)argv;
    if (!SDL_Platform_Init(640, 400, false)) {
        return;
    }

    SDL_Surface* screen = SDL_Platform_Screen();
    if (screen && !Load_VGA_Palette("CCMENU.PAL", screen)) {
        if (!Load_VGA_Palette("TEMPERAT.PAL", screen)) {
            // Fallback palette so the harness stays usable even without CD data.
            SDL_Color pal[256];
            for (int i = 0; i < 256; ++i) {
                pal[i].r = (unsigned char)i;
                pal[i].g = (unsigned char)i;
                pal[i].b = (unsigned char)i;
            }
            pal[0].r = pal[0].g = pal[0].b = 0;
            SDL_SetPalette(screen, SDL_LOGPAL | SDL_PHYSPAL, pal, 0, 256);
        }
    }

    Set_Logic_Page(HidPage);
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
