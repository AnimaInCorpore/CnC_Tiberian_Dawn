#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <chrono>
#include <memory>
#include <vector>

#if defined(TD_PORT_USE_SDL2)
#include <SDL.h>
#endif

#include "ftimer.h"
#include "windows_compat.h"
#include "wintimer_stub.h"

class BufferClass;
class FileClass;
struct PlatformMouseState;
class GraphicViewPortClass;
enum GBC_Enum : std::uint32_t;

// Basic palette-surface wrapper used by the UI code. This is intentionally
// minimal for the in-progress SDL port; it just holds an 8-bit buffer and
// exposes size queries.
class GraphicBufferClass {
 public:
  GraphicBufferClass();
  GraphicBufferClass(int width, int height, void* data = nullptr);
  void Init(int width, int height, void* data = nullptr, int = 0, GBC_Enum flags = GBC_Enum{});
  void Blit(GraphicViewPortClass& dest, int src_x, int src_y, int dst_x, int dst_y, int width, int height) const;

  bool Is_Valid() const;
  int Get_Width() const;
  int Get_Height() const;
  unsigned char* Get_Buffer();
  const unsigned char* Get_Buffer() const;
  void Clear() { std::fill(storage_.begin(), storage_.end(), 0); }
  void Scale(GraphicViewPortClass& dest, int src_x, int src_y, int dst_x, int dst_y, int width,
             int height, int xscale, int yscale, int = 0, char* = nullptr);

 private:
  int width_ = 0;
  int height_ = 0;
  std::vector<unsigned char> storage_;
};

// Legacy keyboard constants used throughout the UI code.
constexpr int KN_NONE = 0x0000;
constexpr int KN_LMOUSE = 0x0100;
constexpr int KN_RMOUSE = 0x0101;
constexpr int KN_RLSE_BIT = 0x2000;
constexpr int KN_BUTTON = 0x8000;
constexpr int KN_SPACE = 0x0020;
constexpr int KN_ESC = 0x001B;
constexpr int KN_LSHIFT = 0x002A;
constexpr int KN_RSHIFT = 0x0036;
constexpr int KN_LCTRL = 0x001D;
constexpr int KN_RCTRL = 0x011D;
constexpr int KN_LALT = 0x0038;
constexpr int KN_RALT = 0x0138;
#if defined(TD_PORT_USE_SDL2)
constexpr int KN_UP = SDLK_UP;
constexpr int KN_DOWN = SDLK_DOWN;
constexpr int KN_LEFT = SDLK_LEFT;
constexpr int KN_RIGHT = SDLK_RIGHT;
constexpr int KN_RETURN = SDLK_RETURN;
#else
constexpr int KN_UP = 0x4800;
constexpr int KN_DOWN = 0x5000;
constexpr int KN_LEFT = 0x4B00;
constexpr int KN_RIGHT = 0x4D00;
constexpr int KN_RETURN = 0x000D;
#endif

// Legacy graphic buffer flags used by the Westwood runtime.
enum GBC_Enum : std::uint32_t {
  GBC_NONE = 0,
  GBC_VISIBLE = 1u << 0,
  GBC_VIDEOMEM = 1u << 1,
};

// Shape drawing flags consumed by the original KEYFBUFF routines.
using ShapeFlags_Type = std::uint32_t;

#ifndef SHAPE_NORMAL
#define SHAPE_NORMAL 0x0000
#endif
#ifndef SHAPE_CENTER
#define SHAPE_CENTER 0x0001
#endif
#ifndef SHAPE_WIN_REL
#define SHAPE_WIN_REL 0x0002
#endif
#ifndef SHAPE_GHOST
#define SHAPE_GHOST 0x0004
#endif
#ifndef SHAPE_FADING
#define SHAPE_FADING 0x0008
#endif
#ifndef SHAPE_PREDATOR
#define SHAPE_PREDATOR 0x0010
#endif
#ifndef SHAPE_PRIORITY
#define SHAPE_PRIORITY 0x0020
#endif
#ifndef SHAPE_TRANS
#define SHAPE_TRANS 0x0040
#endif

// Compatibility shim for the original Westwood 2D viewport wrapper. The modern
// implementation uses a light-weight software surface so UI gadgets can start
// rendering again while the final renderer is still in flight.
class GraphicViewPortClass {
 public:
  GraphicViewPortClass();
  GraphicViewPortClass(GraphicBufferClass* buffer, int x, int y, int width, int height);
  GraphicViewPortClass(GraphicViewPortClass&&) noexcept;
  GraphicViewPortClass& operator=(GraphicViewPortClass&&) noexcept;
  ~GraphicViewPortClass();
  GraphicViewPortClass(const GraphicViewPortClass&) = delete;
  GraphicViewPortClass& operator=(const GraphicViewPortClass&) = delete;

  void Configure(GraphicBufferClass* buffer, int x, int y, int width, int height);

  bool Lock();
  void Unlock();

  void Clear();
  void Fill_Rect(int x1, int y1, int x2, int y2, int color);
  void Draw_Line(int x1, int y1, int x2, int y2, int color);
  void Draw_Rect(int x1, int y1, int x2, int y2, int color);
  void Remap(int x, int y, int width, int height, const unsigned char* table);
  void Put_Pixel(int x, int y, int color);
  int Get_Pixel(int x, int y) const;
  bool Contains(int x, int y) const;

  int Get_XPos() const;
  int Get_YPos() const;
  int Get_Width() const;
  int Get_Height() const;
  GraphicBufferClass* Get_Graphic_Buffer() const;
  bool Get_IsDirectDraw() const;
  void Blit(const GraphicBufferClass& src, int src_x, int src_y, int dst_x, int dst_y, int width, int height);
  void Blit(const GraphicViewPortClass& src, int src_x, int src_y, int dst_x, int dst_y, int width, int height);
  void Blit(const GraphicViewPortClass& src, int dst_x, int dst_y);
  void Blit(const GraphicViewPortClass& src);

 private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};

// Minimal timer shim used by assorted legacy systems.
class TimerClass {
 public:
  TimerClass();
  explicit TimerClass(long ticks);

  void Reset(long ticks = 0);
  void Clear();
  long Time() const;
  bool Expired() const;

 private:
  std::chrono::steady_clock::time_point start_time_;
  long duration_ms_ = 0;
  bool active_ = false;
};

class WWMouseClass {
 public:
  WWMouseClass();
  WWMouseClass(GraphicViewPortClass* page, int width, int height);
  WWMouseClass(WWMouseClass&&) noexcept;
  WWMouseClass& operator=(WWMouseClass&&) noexcept;
  ~WWMouseClass();
  WWMouseClass(const WWMouseClass&) = delete;
  WWMouseClass& operator=(const WWMouseClass&) = delete;

  void Draw_Mouse(GraphicViewPortClass* page);
  void Erase_Mouse(GraphicViewPortClass* page, bool force);
  void Clear_Cursor_Clip();
  void Set_Cursor_Clip();

 private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};

class WWKeyboardClass {
 public:
  WWKeyboardClass();
  WWKeyboardClass(WWKeyboardClass&&) noexcept;
  WWKeyboardClass& operator=(WWKeyboardClass&&) noexcept;
  ~WWKeyboardClass();
  WWKeyboardClass(const WWKeyboardClass&) = delete;
  WWKeyboardClass& operator=(const WWKeyboardClass&) = delete;

  int Get();
  int Check() const;
  void Clear();
  void Stuff(int key);
  bool Down(int key) const;
  void Message_Handler(HWND hwnd, unsigned int message, WPARAM wparam, LPARAM lparam);

  int MouseQX = 0;
  int MouseQY = 0;

 private:
  struct Impl;
  std::unique_ptr<Impl> impl_;

  friend struct PlatformMouseState;
  friend void Platform_Update_Mouse_State(const PlatformMouseState& state);
  friend void Platform_Queue_Key_Event(int key, bool pressed);
};

// Minimal stub for the DirectDraw surface manager used by the original runtime.
struct SurfaceCollectionStub {
  bool SurfacesRestored = false;

  void Set_Surface_Focus(bool) {}
  void Restore_Surfaces() { SurfacesRestored = false; }
  void Release() { SurfacesRestored = false; }
  void Remove_DD_Surface(void*) {}
};

extern SurfaceCollectionStub AllSurfaces;

// The runtime keeps a "current" drawing surface that UI widgets reference.
extern GraphicViewPortClass* LogicPage;
GraphicViewPortClass* Set_Logic_Page(GraphicViewPortClass& page);

// Video mode helpers expected by the legacy launch code.
constexpr int RESET_MODE = -1;
constexpr int MCGA_MODE = 0;
constexpr int DEFAULT_SCREEN_WIDTH = 640;
constexpr int DEFAULT_SCREEN_HEIGHT = 480;

// Timer source hint used by the countdown timers. The original differentiated
// hardware timers; the stub just keeps the symbolic value.
constexpr int BT_SYSTEM = 0;

bool Set_Video_Mode(int mode);
bool Set_Video_Mode(void* window, int width, int height, int bits_per_pixel);

// Keyboard/mouse helpers provided by the original wwlib32 runtime.
int Get_Key_Num();
int Check_Key_Num();
int KN_To_KA(int key);
void Clear_KeyBuffer();
void Stuff_Key_Num(int key);
int Key_Down(int key);

int Get_Mouse_X();
int Get_Mouse_Y();
void Update_Mouse_Position(int x, int y);

void Set_Font_Palette_Range(void const* palette, int first, int count);

void const* Set_Current_Font(void const* font);
void const* Get_Current_Font();
void Set_Gradient_Font_6(void const* font);
void const* Get_Gradient_Font_6();
void Platform_Set_Fonts(const void* current_font, const void* gradient_font6,
                        int font_height, int font_y_spacing);

struct PlatformMouseState {
  int x = 0;
  int y = 0;
  bool left_button_down = false;
  bool right_button_down = false;
};

void Platform_Update_Mouse_State(const PlatformMouseState& state);
void Platform_Queue_Key_Event(int key, bool pressed);
