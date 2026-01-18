#include "sdl_platform.h"

#include <SDL.h>

#include <cstring>

static SDL_Surface* g_screen = 0;
static int g_mouse_x = 0;
static int g_mouse_y = 0;
static int g_mouse_event_x = 0;
static int g_mouse_event_y = 0;
static bool g_left_pressed = false;
static bool g_left_released = false;
static bool g_left_down = false;
static bool g_right_pressed = false;
static bool g_right_released = false;
static bool g_right_down = false;
static int g_key_queue[32];
static int g_key_head = 0;
static int g_key_tail = 0;

static int Map_Key(SDL_keysym const& keysym)
{
    const SDLKey sym = keysym.sym;
    const SDLMod mod = keysym.mod;
    const bool shift = (mod & (KMOD_LSHIFT | KMOD_RSHIFT)) != 0;
    const bool caps = (mod & KMOD_CAPS) != 0;

    switch (sym) {
        case SDLK_ESCAPE: return 27;
        case SDLK_RETURN: return 13;
        case SDLK_KP_ENTER: return 13;
        case SDLK_BACKSPACE: return 8;
        case SDLK_LEFT: return 1000;
        case SDLK_RIGHT: return 1001;
        case SDLK_UP: return 1002;
        case SDLK_DOWN: return 1003;
        default:
            break;
    }

    if (sym == SDLK_SPACE) return ' ';

    if (sym >= SDLK_a && sym <= SDLK_z) {
        const bool upper = shift ^ caps;
        return (upper ? ('A' + (sym - SDLK_a)) : ('a' + (sym - SDLK_a)));
    }

    if (sym >= SDLK_0 && sym <= SDLK_9) {
        static const char shifted[10] = { ')', '!', '@', '#', '$', '%', '^', '&', '*', '(' };
        if (shift) return shifted[sym - SDLK_0];
        return '0' + (sym - SDLK_0);
    }

    if (sym >= SDLK_KP0 && sym <= SDLK_KP9) {
        return '0' + (sym - SDLK_KP0);
    }

    switch (sym) {
        case SDLK_MINUS: return shift ? '_' : '-';
        case SDLK_EQUALS: return shift ? '+' : '=';
        case SDLK_LEFTBRACKET: return shift ? '{' : '[';
        case SDLK_RIGHTBRACKET: return shift ? '}' : ']';
        case SDLK_BACKSLASH: return shift ? '|' : '\\';
        case SDLK_SEMICOLON: return shift ? ':' : ';';
        case SDLK_QUOTE: return shift ? '"' : '\'';
        case SDLK_COMMA: return shift ? '<' : ',';
        case SDLK_PERIOD: return shift ? '>' : '.';
        case SDLK_SLASH: return shift ? '?' : '/';
        case SDLK_BACKQUOTE: return shift ? '~' : '`';
        default:
            break;
    }

    return 0;
}

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

    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

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
    g_right_pressed = false;
    g_right_released = false;

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
                    g_left_down = true;
                    g_left_pressed = true;
                    g_mouse_x = event.button.x;
                    g_mouse_y = event.button.y;
                    g_mouse_event_x = event.button.x;
                    g_mouse_event_y = event.button.y;
                }
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    g_right_down = true;
                    g_right_pressed = true;
                    g_mouse_x = event.button.x;
                    g_mouse_y = event.button.y;
                    g_mouse_event_x = event.button.x;
                    g_mouse_event_y = event.button.y;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    g_left_down = false;
                    g_left_released = true;
                    g_mouse_x = event.button.x;
                    g_mouse_y = event.button.y;
                    g_mouse_event_x = event.button.x;
                    g_mouse_event_y = event.button.y;
                }
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    g_right_down = false;
                    g_right_released = true;
                    g_mouse_x = event.button.x;
                    g_mouse_y = event.button.y;
                    g_mouse_event_x = event.button.x;
                    g_mouse_event_y = event.button.y;
                }
                break;
            case SDL_KEYDOWN: {
                int key = Map_Key(event.key.keysym);
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
int SDL_Platform_Mouse_Event_X() { return g_mouse_event_x; }
int SDL_Platform_Mouse_Event_Y() { return g_mouse_event_y; }
bool SDL_Platform_Mouse_Left_Pressed() { return g_left_pressed; }
bool SDL_Platform_Mouse_Left_Released() { return g_left_released; }
bool SDL_Platform_Mouse_Left_Down() { return g_left_down; }
bool SDL_Platform_Mouse_Right_Pressed() { return g_right_pressed; }
bool SDL_Platform_Mouse_Right_Released() { return g_right_released; }
bool SDL_Platform_Mouse_Right_Down() { return g_right_down; }
int SDL_Platform_Pop_Key() { return Pop_Key(); }
