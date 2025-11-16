#pragma once

#include <cstddef>
#include <cstdint>

class BufferClass;
class GraphicBufferClass;
class WWKeyboardClass;
class WWMouseClass;
class TimerClass;
class CountDownTimerClass;
class WinTimerClass;
class FileClass;

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
// port only needs the method declarations so the migrated translation units can
// compile; the actual definitions will arrive together with the future renderer
// port.
class GraphicViewPortClass {
 public:
  GraphicViewPortClass();
  GraphicViewPortClass(GraphicBufferClass* buffer, int x, int y, int width, int height);
  ~GraphicViewPortClass();

  bool Lock();
  void Unlock();

  void Fill_Rect(int x1, int y1, int x2, int y2, int color);
  void Draw_Line(int x1, int y1, int x2, int y2, int color);

  int Get_Width() const;
  int Get_Height() const;
  GraphicBufferClass* Get_Graphic_Buffer() const;
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

void Set_Font_Palette_Range(void const* palette, int first, int count);
