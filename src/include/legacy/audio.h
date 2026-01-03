#pragma once

// Portable audio layer for the port build.
// This replaces the legacy Win95/DirectSound bindings.

#if defined(TD_PORT_USE_SDL2)
#include <SDL.h>
#include <cstddef>
#include <cstdint>
#endif

// Mirrors the legacy Win95 startup call shape:
//   Audio_Init(hwnd, 16, false, 11025 * 2, 0);
// Meaning: bits-per-sample, stereo flag, sample rate in Hz, and buffer size hint.
bool Audio_Init(void* window_handle, int bits_per_sample, bool stereo, int sample_rate_hz, int buffer_samples);
void Sound_End();

#if defined(TD_PORT_USE_SDL2)
SDL_AudioDeviceID Audio_Get_Device();
SDL_AudioSpec const* Audio_Get_Spec();

void Movie_Audio_Begin(int volume, SDL_AudioFormat src_format, std::uint8_t src_channels, int src_freq);
void Movie_Audio_Push(const std::uint8_t* data, std::size_t len);
void Movie_Audio_End();
#endif
