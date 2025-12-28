#pragma once

// Portable audio layer for the port build.
// This replaces the legacy Win95/DirectSound bindings.

#if defined(TD_PORT_USE_SDL2)
#include <SDL.h>
#endif

// Mirrors the legacy Win95 startup call shape:
//   Audio_Init(hwnd, 16, false, 11025 * 2, 0);
// Meaning: bits-per-sample, stereo flag, sample rate in Hz, and buffer size hint.
bool Audio_Init(void* window_handle, int bits_per_sample, bool stereo, int sample_rate_hz, int buffer_samples);
void Sound_End();

#if defined(TD_PORT_USE_SDL2)
SDL_AudioDeviceID Audio_Get_Device();
SDL_AudioSpec const* Audio_Get_Spec();
#endif
