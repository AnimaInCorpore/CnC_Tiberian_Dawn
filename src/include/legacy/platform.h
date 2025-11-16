#pragma once

#include <cstddef>
#include <cstdint>

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
