#pragma once

#include "platform.h"

// The Watcom toolchain routed most warnings through #pragma warning. For the
// modern GNU/Clang toolchains we expose a lightweight wrapper so individual
// translation units can continue to suppress diagnostics without littering
// build-system specific directives everywhere.
#if defined(__WATCOMC__)
#pragma warning * 0
#pragma warning 549 9
#pragma warning 389 9
#pragma warning 604 9
#pragma warning 595 9
#pragma warning 594 9
#pragma warning 698 9
#pragma warning 665 9
#pragma warning 579 9
#pragma warning 657 9
#pragma warning 472 9
#else
#if defined(__clang__)
#define CNC_WATCOM_PRAGMA(msg) _Pragma(#msg)
#define CNC_WATCOM_DIAGNOSTIC_PUSH _Pragma("clang diagnostic push")
#define CNC_WATCOM_DIAGNOSTIC_POP _Pragma("clang diagnostic pop")
#define CNC_WATCOM_IGNORE_UNUSED _Pragma("clang diagnostic ignored \"-Wunused-parameter\"")
#elif defined(__GNUC__)
#define CNC_WATCOM_PRAGMA(msg) _Pragma(#msg)
#define CNC_WATCOM_DIAGNOSTIC_PUSH _Pragma("GCC diagnostic push")
#define CNC_WATCOM_DIAGNOSTIC_POP _Pragma("GCC diagnostic pop")
#define CNC_WATCOM_IGNORE_UNUSED _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"")
#else
#define CNC_WATCOM_PRAGMA(msg)
#define CNC_WATCOM_DIAGNOSTIC_PUSH
#define CNC_WATCOM_DIAGNOSTIC_POP
#define CNC_WATCOM_IGNORE_UNUSED
#endif
#endif

#ifndef CNC_WATCOM_UNUSED
#define CNC_WATCOM_UNUSED(param) ((void)(param))
#endif
