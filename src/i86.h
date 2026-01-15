/* Minimal shim for legacy i86.h expectations in the portable build. */
#pragma once

typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef unsigned int UINT;
typedef unsigned long DWORD;

/* No-op macros that existed for Watcom/MSVC intrinsics in the original code. */
#ifndef __far
#define __far
#endif

#ifndef __near
#define __near
#endif
