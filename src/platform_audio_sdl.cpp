// SDL2 audio device management for the portable build.

#include "legacy/audio.h"

#include <SDL.h>
#include <cstdio>

namespace {
SDL_AudioDeviceID g_audio_device = 0;
SDL_AudioSpec g_audio_spec{};
}  // namespace

extern "C" void Audio_Mix_Callback(void* userdata, Uint8* stream, int len);

bool Audio_Init(void* /*window_handle*/, int bits_per_sample, bool stereo, int sample_rate_hz, int buffer_samples) {
  if (SDL_WasInit(SDL_INIT_AUDIO) == 0) {
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
      std::fprintf(stderr, "SDL_InitSubSystem(SDL_INIT_AUDIO) failed: %s\n", SDL_GetError());
      return false;
    }
  }

  SDL_AudioSpec desired{};
  desired.freq = sample_rate_hz > 0 ? sample_rate_hz : 22050;
  desired.format = (bits_per_sample >= 16) ? AUDIO_S16SYS : AUDIO_U8;
  desired.channels = stereo ? 2 : 1;
  desired.samples = buffer_samples > 0 ? static_cast<Uint16>(buffer_samples) : 2048;
  desired.callback = Audio_Mix_Callback;
  desired.userdata = nullptr;

  SDL_AudioDeviceID device = SDL_OpenAudioDevice(
      nullptr, 0, &desired, &g_audio_spec,
      SDL_AUDIO_ALLOW_FREQUENCY_CHANGE | SDL_AUDIO_ALLOW_FORMAT_CHANGE | SDL_AUDIO_ALLOW_CHANNELS_CHANGE |
          SDL_AUDIO_ALLOW_SAMPLES_CHANGE);
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
