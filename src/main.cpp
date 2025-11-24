#include <SDL2/SDL.h>
#include "platform_input.h"
#include "game.h"

void Game_Startup(int argc, char* argv[]);
void Game_Shutdown(void);
void Main_Game(int argc, char* argv[]);

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow(
        "Command & Conquer",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        640,
        480,
        SDL_WINDOW_SHOWN
    );

    Game_Startup(argc, argv);
    Main_Game(argc, argv);

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            Platform_Handle_Sdl_Event(event);
        }
    }

    Game_Shutdown();
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
