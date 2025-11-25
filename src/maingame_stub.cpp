#include <SDL2/SDL.h>
#include <cstdio>

// Legacy flag polled by shutdown code; defined in port_stubs.cpp.
extern bool ReadyToQuit;

namespace {
void Push_Quit_Event() {
    SDL_Event quit_event{};
    quit_event.type = SDL_QUIT;
    SDL_PushEvent(&quit_event);
}
}  // namespace

int Main_Game(int argc, char** argv) {
    std::printf("Main_Game: Stub implementation called.\n");
    std::printf("Arguments received: %d\n", argc);
    for (int i = 0; i < argc; ++i) {
        std::printf("  argv[%d]: %s\n", i, argv[i]);
    }

    // Until the real gameplay loop is ported, immediately request shutdown.
    ReadyToQuit = true;
    Push_Quit_Event();
    return 0;
}
