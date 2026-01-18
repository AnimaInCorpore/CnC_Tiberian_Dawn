#pragma once

#include "legacy_compat.h"

struct SDL_Surface;

bool SDL_Platform_Init(int width, int height, bool fullscreen);
void SDL_Platform_Shutdown();

bool SDL_Platform_Pump_Events(bool& should_quit);
void SDL_Platform_Clear(unsigned char r, unsigned char g, unsigned char b);
void SDL_Platform_Present();
void SDL_Platform_Present_Indexed8(unsigned char const* src, int width, int height, int pitch);

unsigned long SDL_Platform_Ticks();
void SDL_Platform_Delay(unsigned long ms);

// Optional helpers used by the shim UI/graphics layer.
SDL_Surface* SDL_Platform_Screen();
int SDL_Platform_Mouse_X();
int SDL_Platform_Mouse_Y();
int SDL_Platform_Mouse_Event_X();
int SDL_Platform_Mouse_Event_Y();
bool SDL_Platform_Mouse_Left_Pressed();
bool SDL_Platform_Mouse_Left_Released();
bool SDL_Platform_Mouse_Left_Down();
bool SDL_Platform_Mouse_Right_Pressed();
bool SDL_Platform_Mouse_Right_Released();
bool SDL_Platform_Mouse_Right_Down();
int SDL_Platform_Pop_Key();
