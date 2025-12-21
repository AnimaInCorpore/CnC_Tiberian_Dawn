#ifndef AUDIO_STUB_H
#define AUDIO_STUB_H

// Dummy declarations for legacy audio functions
bool Audio_Init(void* param1, int param2, bool param3, int param4, int param5);
void Sound_End();

#if defined(TD_PORT_USE_SDL2)
#include <SDL.h>
SDL_AudioDeviceID Audio_Get_Device();
SDL_AudioSpec const* Audio_Get_Spec();
#endif

#endif // AUDIO_STUB_H
