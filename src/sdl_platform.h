#pragma once

#include "legacy_compat.h"

struct SDL_Surface;

bool SDL_Platform_Init(int width, int height, bool fullscreen);
void SDL_Platform_Shutdown();

bool SDL_Platform_Pump_Events(bool& should_quit);
void SDL_Platform_Clear(unsigned char r, unsigned char g, unsigned char b);
void SDL_Platform_Present();

unsigned long SDL_Platform_Ticks();
void SDL_Platform_Delay(unsigned long ms);

