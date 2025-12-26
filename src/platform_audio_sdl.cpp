// SDL2 audio device management for the portable build.

#include "legacy/audio.h"

#include <SDL.h>
#include <cstdio>

namespace {
SDL_AudioDeviceID g_audio_device = 0;
SDL_AudioSpec g_audio_spec{};
}  // namespace

extern "C" void Audio_Mix_Callback(void* userdata, Uint8* stream, int len);

bool Audio_Init(void* /*window_handle*/, int rate, bool sixteen_bit, int num_channels, int audio_buffer_size) {
  if (SDL_WasInit(SDL_INIT_AUDIO) == 0) {
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
      std::fprintf(stderr, "SDL_InitSubSystem(SDL_INIT_AUDIO) failed: %s\n", SDL_GetError());
      return false;
    }
  }

  SDL_AudioSpec desired{};
  desired.freq = rate > 0 ? rate : 44100;
  desired.format = sixteen_bit ? AUDIO_S16SYS : AUDIO_U8;
  desired.channels = num_channels > 0 ? static_cast<Uint8>(num_channels) : 2;
  desired.samples = audio_buffer_size > 0 ? static_cast<Uint16>(audio_buffer_size) : 2048;
  desired.callback = Audio_Mix_Callback;
  desired.userdata = nullptr;

  SDL_AudioDeviceID device = SDL_OpenAudioDevice(nullptr, 0, &desired, &g_audio_spec, 0);
  if (device == 0) {
    std::fprintf(stderr, "SDL_OpenAudioDevice failed: %s\n", SDL_GetError());
    return false;
  }

  g_audio_device = device;
  SDL_PauseAudioDevice(g_audio_device, 0);  // Start playback (silence until mixed).
  return true;
}

void Sound_End() {
  if (g_audio_device != 0) {
    SDL_CloseAudioDevice(g_audio_device);
    g_audio_device = 0;
  }
  if (SDL_WasInit(SDL_INIT_AUDIO) != 0) {
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
  }
}

SDL_AudioDeviceID Audio_Get_Device() { return g_audio_device; }

SDL_AudioSpec const* Audio_Get_Spec() { return &g_audio_spec; }

