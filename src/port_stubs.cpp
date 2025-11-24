#include "legacy/compat.h"
#include "legacy/function.h"
#include "legacy/nullmodem_stub.h"
#include "legacy/windows_compat.h"

#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <string>

// Basic font metrics used by the UI code.
int FontHeight = 8;
int FontYSpacing = 1;

// Global ready flag and null modem instance expected by legacy code paths.
bool ReadyToQuit = false;
NullModemClass NullModem;

// Shape buffer placeholders.
long _ShapeBufferSize = 512 * 1024;
char* _ShapeBuffer = nullptr;

int Bound(int value, int min, int max) {
  if (value < min) return min;
  if (value > max) return max;
  return value;
}

void CCDebugString(char* string) {
  if (string) {
    std::fputs(string, stderr);
  }
}

void Mono_Clear_Screen() {}

int Mono_Printf(char const* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int result = std::vfprintf(stderr, fmt, args);
  std::fputc('\n', stderr);
  va_end(args);
  return result;
}

void* Load_Alloc_Data(FileClass& file) {
  const int size = file.Size();
  if (size <= 0) return nullptr;
  char* buffer = new char[size + 1];
  file.Read(buffer, size);
  buffer[size] = '\0';
  return buffer;
}

int String_Pixel_Width(char const* text) {
  if (!text) return 0;
  return static_cast<int>(std::strlen(text) * 8);
}

int Char_Pixel_Width(int) { return 8; }

void Fancy_Text_Print(char const*, unsigned int, unsigned int, unsigned int,
                     unsigned int, TextPrintType, ...) {
}

void Fancy_Text_Print(int, unsigned int, unsigned int, unsigned int,
                     unsigned int, TextPrintType, ...) {
}

void Conquer_Clip_Text_Print(char const*, unsigned int, unsigned int,
                            unsigned int, unsigned int, TextPrintType,
                            unsigned int, int const*) {
}

void CC_Draw_Shape(void const*, int, int, int, WindowNumberType, unsigned int,
                   void const*, void const*) {}

unsigned short Get_Build_Frame_Width(void const*) { return 0; }
unsigned short Get_Build_Frame_Height(void const*) { return 0; }
int Get_Resolution_Factor() { return 1; }

int Distance_Coord(COORDINATE coord1, COORDINATE coord2) {
  const int dx =
      static_cast<int>(Coord_X(coord2)) - static_cast<int>(Coord_X(coord1));
  const int dy =
      static_cast<int>(Coord_Y(coord2)) - static_cast<int>(Coord_Y(coord1));
  return static_cast<int>(std::sqrt(static_cast<double>(dx * dx + dy * dy)));
}

bool Queue_Options() { return false; }

char const* Extract_String(char const* text, int) { return text; }

void const* Hires_Retrieve(char*) { return nullptr; }

void Validate_Error(char*) {}

bool Parse_Command_Line(int, char**) { return true; }

void Read_Setup_Options(RawFileClass*) {}

int WWGetPrivateProfileInt(char const*, char const*, int def, char*) {
  return def;
}

char* WWGetPrivateProfileString(char const*, char const*, char const* def,
                              char* dst, int dstlen, char*) {
  if (!dst || dstlen <= 0) return nullptr;
  std::strncpy(dst, def ? def : "", dstlen);
  dst[dstlen - 1] = '\0';
  return dst;
}

bool WWWritePrivateProfileString(char const*, char const*, char const* value,
                                char*) {
  return value ? true : false;
}

bool Read_Private_Config_Struct(char*, NewConfigType* config) {
  if (config) {
    std::memset(config, 0, sizeof(*config));
  }
  return true;
}

void Check_Use_Compressed_Shapes() {}

unsigned long Disk_Space_Available() { return 1024 * 1024 * 1024; }

int Ram_Free() { return 16 * 1024 * 1024; }

void Memory_Error_Handler(void) {}

COORDINATE As_Coord(TARGET) { return 0; }
ObjectClass* As_Object(TARGET) { return nullptr; }
BuildingClass* As_Building(TARGET) { return nullptr; }

void Draw_Box(int, int, int, int, BoxStyleEnum, bool) {}

void Conquer_Init_Fonts() {}

void CC_Draw_Text(int) {}

void Game_Startup(void*, int, int, int, bool) {}
