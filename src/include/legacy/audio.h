#pragma once

// Portable audio layer for the port build.
// This replaces the legacy Win95/DirectSound bindings.

#if defined(TD_PORT_USE_SDL2)
#include <SDL.h>
#endif

bool Audio_Init(void* window_handle, int rate, bool sixteen_bit, int num_channels, int audio_buffer_size);
void Sound_End();

#if defined(TD_PORT_USE_SDL2)
SDL_AudioDeviceID Audio_Get_Device();
SDL_AudioSpec const* Audio_Get_Spec();
#endif

