#include "platform_input.h"

#include "runtime_sdl.h"

#include <cstdint>

namespace {
PlatformMouseState g_mouse_state{};

PlatformMouseState Build_Mouse_State(int x, int y, bool left_down, bool right_down) {
  g_mouse_state.x = x;
  g_mouse_state.y = y;
  g_mouse_state.left_button_down = left_down;
  g_mouse_state.right_button_down = right_down;
  return g_mouse_state;
}

#if defined(TD_PORT_USE_SDL2)
void Window_To_Logical_Coords(int in_x, int in_y, int& out_x, int& out_y) {
  SDL_Renderer* renderer = Runtime_Get_Sdl_Renderer();
  if (!renderer) {
    out_x = in_x;
    out_y = in_y;
    return;
  }
  float lx = static_cast<float>(in_x);
  float ly = static_cast<float>(in_y);
  SDL_RenderWindowToLogical(renderer, in_x, in_y, &lx, &ly);
  out_x = static_cast<int>(lx + 0.5f);
  out_y = static_cast<int>(ly + 0.5f);
}
#endif

}  // namespace

bool Platform_Handle_Host_Event(const PlatformInputEvent& event) {
  switch (event.type) {
    case PlatformEventType::MouseMotion:
    case PlatformEventType::MouseButton:
      Platform_Update_Mouse_State(event.mouse);
      return true;
    case PlatformEventType::Key:
      Platform_Queue_Key_Event(event.key, event.pressed);
      return true;
  }
  return false;
}

void Platform_Handle_Host_Events(const PlatformInputEvent* events, std::size_t count) {
  if (!events) {
    return;
  }
  for (std::size_t index = 0; index < count; ++index) {
    Platform_Handle_Host_Event(events[index]);
  }
}

#if defined(TD_PORT_USE_SDL2)
bool Platform_Handle_Sdl_Event(const SDL_Event& event) {
  switch (event.type) {
    case SDL_MOUSEMOTION: {
      const Uint32 state = event.motion.state;
      int x = event.motion.x;
      int y = event.motion.y;
      Window_To_Logical_Coords(x, y, x, y);
      PlatformMouseState mouse_state = Build_Mouse_State(
          x, y,
          (state & SDL_BUTTON_LMASK) != 0,
          (state & SDL_BUTTON_RMASK) != 0);
      Platform_Update_Mouse_State(mouse_state);
      return true;
    }
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP: {
      const bool pressed = event.type == SDL_MOUSEBUTTONDOWN;
      bool left_down = g_mouse_state.left_button_down;
      bool right_down = g_mouse_state.right_button_down;
      if (event.button.button == SDL_BUTTON_LEFT) {
        left_down = pressed;
        Platform_Queue_Key_Event(KN_LMOUSE, pressed);
      } else if (event.button.button == SDL_BUTTON_RIGHT) {
        right_down = pressed;
        Platform_Queue_Key_Event(KN_RMOUSE, pressed);
      }
      int x = event.button.x;
      int y = event.button.y;
      Window_To_Logical_Coords(x, y, x, y);
      Platform_Update_Mouse_State(
          Build_Mouse_State(x, y, left_down, right_down));
      return true;
    }
    case SDL_KEYDOWN:
    case SDL_KEYUP: {
      if (event.key.repeat != 0) {
        return true;
      }
      const bool pressed = event.type == SDL_KEYDOWN;
      Platform_Queue_Key_Event(static_cast<int>(event.key.keysym.sym), pressed);
      return true;
    }
    default:
      break;
  }
  return false;
}
#endif
