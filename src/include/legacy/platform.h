#pragma once

#include <cstddef>
#include <cstdint>

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <windows.h>
#else

// Classic Watcom/MSVCRT-style path constants expected by the legacy code.
#ifndef _MAX_PATH
#define _MAX_PATH 260
#endif
#ifndef _MAX_FNAME
#define _MAX_FNAME 256
#endif
#ifndef _MAX_EXT
#define _MAX_EXT 256
#endif
#ifndef _MAX_DIR
#define _MAX_DIR 256
#endif
#ifndef _MAX_DRIVE
#define _MAX_DRIVE 3
#endif

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

#ifndef MAKEWORD
#define MAKEWORD(low, high) \
  (static_cast<std::uint16_t>(((static_cast<std::uint16_t>(low) & 0x00FFu) | \
                               ((static_cast<std::uint16_t>(high) & 0x00FFu) << 8))))
#endif

#ifndef MAKELONG
#define MAKELONG(low, high) \
  (static_cast<std::uint32_t>(((static_cast<std::uint32_t>(low) & 0x0000FFFFul)) | \
                              ((static_cast<std::uint32_t>(high) & 0x0000FFFFul) << 16)))
#endif

#ifndef LOWORD
#define LOWORD(value) (static_cast<std::uint16_t>((static_cast<std::uint32_t>(value)) & 0xFFFFu))
#endif

#ifndef HIWORD
#define HIWORD(value) \
  (static_cast<std::uint16_t>((static_cast<std::uint32_t>(value) >> 16) & 0xFFFFu))
#endif

// Basic Win16/Watcom compatible typedefs used throughout the legacy codebase.
#ifndef CNC_TD_LEGACY_WORD_DEFINED
using WORD = std::uint16_t;
using DWORD = std::uint32_t;
using BYTE = std::uint8_t;
using BOOL = int;
using UINT = unsigned int;
using LONG = std::int32_t;
using ULONG = std::uint32_t;
using SHORT = std::int16_t;
using USHORT = std::uint16_t;
using UCHAR = unsigned char;
#define CNC_TD_LEGACY_WORD_DEFINED 1
#endif
#endif

#ifndef MAKE_LONG
#define MAKE_LONG(high, low) MAKELONG((low), (high))
#endif

#ifndef LOW_WORD
#define LOW_WORD(value) LOWORD(value)
#endif

#ifndef HIGH_WORD
#define HIGH_WORD(value) HIWORD(value)
#endif

// The DOS build relied on segmented memory keywords. They become no-ops here.
#ifndef near
#define near
#endif
#ifndef far
#define far
#endif
#ifndef __near
#define __near
#endif
#ifndef __far
#define __far
#endif
#ifndef huge
#define huge
#endif
#ifndef __huge
#define __huge
#endif
#ifndef pascal
#define pascal
#endif
#ifndef _pascal
#define _pascal
#endif
#ifndef __pascal
#define __pascal
#endif
#ifndef __interrupt
#define __interrupt
#endif
#ifndef cdecl
#define cdecl
#endif
#ifndef __cdecl
#define __cdecl
#endif

// Helper macros for explicit fallthrough markers when porting old switch logic.
#define CNC_LEGACY_FALLTHROUGH __attribute__((fallthrough))
