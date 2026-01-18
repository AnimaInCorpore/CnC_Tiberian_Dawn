#include "sdl_platform.h"

#include <SDL.h>

#include <cstring>

static SDL_Surface* g_screen = 0;
static int g_mouse_x = 0;
static int g_mouse_y = 0;
static bool g_left_pressed = false;
static bool g_left_released = false;
static int g_key_queue[32];
static int g_key_head = 0;
static int g_key_tail = 0;

static void Push_Key(int key)
{
    int next_tail = (g_key_tail + 1) % (int)(sizeof(g_key_queue) / sizeof(g_key_queue[0]));
    if (next_tail == g_key_head) return;
    g_key_queue[g_key_tail] = key;
    g_key_tail = next_tail;
}

static int Pop_Key()
{
    if (g_key_head == g_key_tail) return 0;
    int key = g_key_queue[g_key_head];
    g_key_head = (g_key_head + 1) % (int)(sizeof(g_key_queue) / sizeof(g_key_queue[0]));
    return key;
}

bool SDL_Platform_Init(int width, int height, bool fullscreen)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        return false;
    }

    const int bpp = 8;
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
    g_left_pressed = false;
    g_left_released = false;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            should_quit = true;
        }
        switch (event.type) {
            case SDL_MOUSEMOTION:
                g_mouse_x = event.motion.x;
                g_mouse_y = event.motion.y;
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    g_left_pressed = true;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    g_left_released = true;
                }
                break;
            case SDL_KEYDOWN: {
                int key = 0;
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE: key = 27; break;
                    case SDLK_RETURN: key = 13; break;
                    case SDLK_LEFT: key = 1000; break;
                    case SDLK_RIGHT: key = 1001; break;
                    case SDLK_UP: key = 1002; break;
                    case SDLK_DOWN: key = 1003; break;
                    default:
                        break;
                }
                if (key) Push_Key(key);
            } break;
            default:
                break;
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

void SDL_Platform_Present_Indexed8(unsigned char const* src, int width, int height, int pitch)
{
    if (!g_screen) return;
    if (!src) {
        SDL_Platform_Present();
        return;
    }

    SDL_Surface* screen = g_screen;
    if (SDL_MUSTLOCK(screen)) {
        if (SDL_LockSurface(screen) != 0) return;
    }

    if (screen->pixels) {
        unsigned char* dst = (unsigned char*)screen->pixels;
        const int copy_w = (width < screen->w) ? width : screen->w;
        const int copy_h = (height < screen->h) ? height : screen->h;
        const int dst_pitch = screen->pitch;
        for (int y = 0; y < copy_h; ++y) {
            std::memcpy(dst + y * dst_pitch, src + y * pitch, (size_t)copy_w);
        }
    }

    if (SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
    SDL_Platform_Present();
}

unsigned long SDL_Platform_Ticks()
{
    return SDL_GetTicks();
}

void SDL_Platform_Delay(unsigned long ms)
{
    SDL_Delay(ms);
}

SDL_Surface* SDL_Platform_Screen() { return g_screen; }
int SDL_Platform_Mouse_X() { return g_mouse_x; }
int SDL_Platform_Mouse_Y() { return g_mouse_y; }
bool SDL_Platform_Mouse_Left_Pressed() { return g_left_pressed; }
bool SDL_Platform_Mouse_Left_Released() { return g_left_released; }
int SDL_Platform_Pop_Key() { return Pop_Key(); }
