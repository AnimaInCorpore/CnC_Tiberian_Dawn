#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>

class BufferClass;
class GraphicBufferClass;
class WWKeyboardClass;
class WWMouseClass;
class TimerClass;
class CountDownTimerClass;
class WinTimerClass;
class FileClass;

// Legacy keyboard constants used throughout the UI code.
constexpr int KN_NONE = 0x0000;
constexpr int KN_LMOUSE = 0x0100;
constexpr int KN_RMOUSE = 0x0101;
constexpr int KN_RLSE_BIT = 0x2000;
constexpr int KN_SPACE = 0x0020;
constexpr int KN_ESC = 0x001B;

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

  void Fill_Rect(int x1, int y1, int x2, int y2, int color);
  void Draw_Line(int x1, int y1, int x2, int y2, int color);
  void Draw_Rect(int x1, int y1, int x2, int y2, int color);
  void Put_Pixel(int x, int y, int color);
  int Get_Pixel(int x, int y) const;
  bool Contains(int x, int y) const;

  int Get_XPos() const;
  int Get_YPos() const;
  int Get_Width() const;
  int Get_Height() const;
  GraphicBufferClass* Get_Graphic_Buffer() const;

 private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
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

// The runtime keeps a "current" drawing surface that UI widgets reference.
extern GraphicViewPortClass* LogicPage;
GraphicViewPortClass* Set_Logic_Page(GraphicViewPortClass& page);

// Video mode helpers expected by the legacy launch code.
constexpr int RESET_MODE = -1;
constexpr int MCGA_MODE = 0;

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
