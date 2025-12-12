#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#ifndef _WIN32
#include <strings.h>
#endif
#include <string>
#include <cstdio>
#include <climits>

#include "platform.h"

#if defined(_WIN32)
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#ifndef PATH_MAX
#define PATH_MAX MAX_PATH
#endif
#endif

#ifndef _WIN32
#ifndef stricmp
#define stricmp strcasecmp
#endif
#ifndef strnicmp
#define strnicmp strncasecmp
#endif
#endif

class BufferClass;

using KeyNumType = int;
using KeyASCIIType = int;

// Simplified EGA palette constants used throughout the legacy UI code.
constexpr unsigned BLACK = 0;
constexpr unsigned BLUE = 1;
constexpr unsigned GREEN = 2;
constexpr unsigned CYAN = 3;
constexpr unsigned RED = 4;
constexpr unsigned MAGENTA = 5;
constexpr unsigned BROWN = 6;
constexpr unsigned LTGREY = 7;
constexpr unsigned DKGREY = 8;
constexpr unsigned GREY = LTGREY;
constexpr unsigned LTBLUE = 9;
constexpr unsigned LTGREEN = 10;
constexpr unsigned LTCYAN = 11;
constexpr unsigned LTRED = 12;
constexpr unsigned LTMAGENTA = 13;
constexpr unsigned YELLOW = 14;
constexpr unsigned PINK = LTMAGENTA;
constexpr unsigned WHITE = 15;
constexpr unsigned PURPLE = LTMAGENTA;
constexpr unsigned GRAY = GREY;
constexpr unsigned DKGRAY = DKGREY;
constexpr unsigned LTGRAY = LTGREY;
constexpr unsigned DKGREEN = GREEN;
constexpr unsigned DKBLUE = BLUE;

constexpr unsigned TBLACK = BLACK;
constexpr unsigned TBLUE = BLUE;
constexpr unsigned TGREEN = GREEN;
constexpr unsigned TCYAN = CYAN;
constexpr unsigned TRED = RED;
constexpr unsigned TMAGENTA = MAGENTA;
constexpr unsigned TBROWN = BROWN;
constexpr unsigned TLTGREY = LTGREY;
constexpr unsigned TDKGREY = DKGREY;
constexpr unsigned TLTBLUE = LTBLUE;
constexpr unsigned TLTGREEN = LTGREEN;
constexpr unsigned TLTCYAN = LTCYAN;
constexpr unsigned TLTRED = LTRED;
constexpr unsigned TLTMAGENTA = LTMAGENTA;
constexpr unsigned TYELLOW = YELLOW;
constexpr unsigned TWHITE = WHITE;

#ifndef BuffType
#define BuffType BufferClass
#endif

#ifndef movmem
#define movmem(src, dst, count) std::memmove((dst), (src), (count))
#endif

extern long _ShapeBufferSize;
extern char* _ShapeBuffer;
extern bool OverlappedVideoBlits;

#ifndef ShapeBufferSize
#define ShapeBufferSize _ShapeBufferSize
#endif
#ifndef ShapeBuffer
#define ShapeBuffer _ShapeBuffer
#endif

#ifndef HIDBUFF
#define HIDBUFF reinterpret_cast<void*>(0xA0000)
#endif

#ifndef Set_Font_Palette
#define Set_Font_Palette(palette) Set_Font_Palette_Range((palette), 0, 15)
#endif

#ifndef READ
#define READ 1
#endif
#ifndef WRITE
#define WRITE 2
#endif

#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif

#ifndef ERROR_WINDOW
#define ERROR_WINDOW 1
#endif
#ifndef ErrorWindow
#define ErrorWindow 1
#endif

extern unsigned char* Palette;
extern unsigned char MDisabled;
extern WORD Hard_Error_Occured;
extern int WindowList[][8];

enum WindowListIndex {
  WINDOWX,
  WINDOWY,
  WINDOWWIDTH,
  WINDOWHEIGHT,
  WINDOWCURSORCOLOR,
  WINDOWBKCOLOR,
  WINDOWCURSORX,
  WINDOWCURSORY
};

inline void Mem_Copy(void* dest, const void* src, std::size_t count) {
  if (!dest || !src || count == 0) return;
  std::memmove(dest, src, count);
}

typedef enum MenuIndexType {
  MENUX,
  MENUY,
  ITEMWIDTH,
  ITEMSHIGH,
  MSELECTED,
  NORMCOL,
  HILITE,
  MENUPADDING = 0x1000
} MenuIndexType;

#define BITSPERBYTE 8
#define MAXSHORT 0x7fff
#define HIBITS 0x8000
#ifndef MAXLONG
#define MAXLONG 0x7fffffffL
#endif
#define HIBITL 0x80000000

#ifndef MAXINT
#define MAXINT MAXLONG
#endif
#define HIBITI HIBITL

#define DMAXEXP 308
#define FMAXEXP 38
#define DMINEXP -307
#define FMINEXP -37

#ifndef MAXDOUBLE
#define MAXDOUBLE 1.797693E+308
#endif
#ifndef MAXFLOAT
#define MAXFLOAT 3.37E+38F
#endif
#ifndef MINDOUBLE
#define MINDOUBLE 2.225074E-308
#endif
#ifndef MINFLOAT
#define MINFLOAT 8.43E-37F
#endif

#define DSIGNIF 53
#define FSIGNIF 24

#define DMAXPOWTWO 0x3FF
#define FMAXPOWTWO 0x7F
#define DEXPLEN 11
#define FEXPLEN 8
#define EXPBASE 2
#define IEEE 1
#define LENBASE 1
#define HIDDENBIT 1
#define LN_MAXDOUBLE 7.0978E+2
#define LN_MINDOUBLE -7.0840E+2

#ifndef ABS
#define ABS(x) (((x) >= 0) ? (x) : -(x))
#endif

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

bool Confine_Rect(int* x, int* y, int width, int height, int max_width, int max_height);

void Stuff_Key_Num(int value);

extern "C" {
extern int MouseQX;
extern int MouseQY;
}
