#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <cstdio>

#include "game.h"
#include "platform_input.h"
#include "runtime_sdl.h"
#include "legacy/function.h"
#include "port_debug.h"

// Global shutdown flag maintained by legacy code paths.
extern bool ReadyToQuit;

void Game_Startup(int argc, char* argv[]);
void Game_Shutdown(void);
void Main_Game(int argc, char* argv[]);

int main(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        if (!argv[i]) continue;
        if (std::strcmp(argv[i], "--verbose") == 0 || std::strcmp(argv[i], "-v") == 0 ||
            std::strcmp(argv[i], "--debug") == 0) {
            SDL_setenv("TD_VERBOSE", "1", 1);
            break;
        }
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");  // avoid bilinear blur when scaling

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    TD_Debugf("SDL version: %u.%u.%u", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);
    TD_Debugf("SDL video driver: %s", SDL_GetCurrentVideoDriver() ? SDL_GetCurrentVideoDriver() : "(null)");
    TD_Debugf("SDL_RENDER_DRIVER=%s", SDL_getenv("SDL_RENDER_DRIVER") ? SDL_getenv("SDL_RENDER_DRIVER") : "(unset)");
    TD_Debugf("SDL_VIDEODRIVER=%s", SDL_getenv("SDL_VIDEODRIVER") ? SDL_getenv("SDL_VIDEODRIVER") : "(unset)");
    const int render_drivers = SDL_GetNumRenderDrivers();
    TD_Debugf("SDL render drivers available: %d", render_drivers);
    for (int i = 0; i < render_drivers; ++i) {
        SDL_RendererInfo info{};
        if (SDL_GetRenderDriverInfo(i, &info) == 0) {
            TD_Debugf("  driver[%d]=%s flags=0x%x texfmts=%u max=%dx%d",
                     i,
                     info.name ? info.name : "(null)",
                     static_cast<unsigned>(info.flags),
                     info.num_texture_formats,
                     info.max_texture_width,
                     info.max_texture_height);
        } else {
            TD_Debugf("  driver[%d]=<error: %s>", i, SDL_GetError());
        }
    }

    const Uint32 window_flags = SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI;
    SDL_Window* window = SDL_CreateWindow(
        "Command & Conquer",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        ScreenWidth,
        ScreenHeight,
        window_flags
    );
    if (!window) {
        std::fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    TD_Debugf("SDL_CreateWindow OK (%dx%d)", ScreenWidth, ScreenHeight);

    SDL_Renderer* renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        // Headless/dummy drivers may not expose accelerated renderers; fall back to software.
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    }
    if (!renderer) {
        std::fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_RendererInfo chosen{};
    if (SDL_GetRendererInfo(renderer, &chosen) == 0) {
        TD_Debugf("SDL renderer: %s flags=0x%x max=%dx%d",
                 chosen.name ? chosen.name : "(null)",
                 static_cast<unsigned>(chosen.flags),
                 chosen.max_texture_width,
                 chosen.max_texture_height);
    } else {
        TD_Debugf("SDL_GetRendererInfo failed: %s", SDL_GetError());
    }

    if (SDL_RenderSetLogicalSize(renderer, ScreenWidth, ScreenHeight) != 0) {
        std::fprintf(stderr, "SDL_RenderSetLogicalSize failed: %s\n", SDL_GetError());
    }
    SDL_RenderSetIntegerScale(renderer, SDL_TRUE);

    Runtime_Set_Sdl_Window(window);
    Runtime_Set_Sdl_Renderer(renderer);

    TD_Debugf("Entering Game_Startup...");
    Game_Startup(argc, argv);
    TD_Debugf("Entering Main_Game...");
    Main_Game(argc, argv);
    TD_Debugf("Entering Game_Shutdown...");
    Game_Shutdown();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
