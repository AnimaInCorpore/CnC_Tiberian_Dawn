#pragma once

#include <cstddef>

#include "wwlib32.h"

enum class PlatformEventType {
  MouseMotion,
  MouseButton,
  Key,
};

struct PlatformInputEvent {
  PlatformEventType type = PlatformEventType::MouseMotion;
  PlatformMouseState mouse{};
  int key = 0;
  bool pressed = false;
};

bool Platform_Handle_Host_Event(const PlatformInputEvent& event);
void Platform_Handle_Host_Events(const PlatformInputEvent* events, std::size_t count);

#if defined(TD_PORT_USE_SDL2)
#include <SDL.h>

bool Platform_Handle_Sdl_Event(const SDL_Event& event);
#endif
