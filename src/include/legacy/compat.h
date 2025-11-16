#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>

#include "platform.h"

class BufferClass;

using KeyNumType = int;
using KeyASCIIType = int;

#ifndef BuffType
#define BuffType BufferClass
#endif

#ifndef movmem
#define movmem(src, dst, count) std::memmove((dst), (src), (count))
#endif

extern long _ShapeBufferSize;
extern char* _ShapeBuffer;

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

#define MAXINT MAXLONG
#define HIBITI HIBITL

#define DMAXEXP 308
#define FMAXEXP 38
#define DMINEXP -307
#define FMINEXP -37

#define MAXDOUBLE 1.797693E+308
#define MAXFLOAT 3.37E+38F
#define MINDOUBLE 2.225074E-308
#define MINFLOAT 8.43E-37F

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

#define DKGREEN GREEN
#define DKBLUE BLUE
#define GRAY GREY
#define DKGREY GREY
#define DKGRAY GREY
#define LTGRAY LTGREY

void Stuff_Key_Num(int value);

extern "C" {
extern int MouseQX;
extern int MouseQY;
}
