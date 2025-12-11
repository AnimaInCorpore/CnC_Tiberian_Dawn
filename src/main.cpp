#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <cstdio>

#include "game.h"
#include "platform_input.h"
#include "runtime_sdl.h"
#include "legacy/function.h"

// Global shutdown flag maintained by legacy code paths.
extern bool ReadyToQuit;

void Game_Startup(int argc, char* argv[]);
void Game_Shutdown(void);
void Main_Game(int argc, char* argv[]);

int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Command & Conquer",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        ScreenWidth,
        ScreenHeight,
        SDL_WINDOW_SHOWN
    );
    if (!window) {
        std::fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

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

    if (SDL_RenderSetLogicalSize(renderer, ScreenWidth, ScreenHeight) != 0) {
        std::fprintf(stderr, "SDL_RenderSetLogicalSize failed: %s\n", SDL_GetError());
    }
    SDL_RenderSetIntegerScale(renderer, SDL_TRUE);

    Runtime_Set_Sdl_Window(window);
    Runtime_Set_Sdl_Renderer(renderer);

    Game_Startup(argc, argv);
    Main_Game(argc, argv);
    Game_Shutdown();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}


