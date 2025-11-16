#pragma once

#include "platform.h"

// The Watcom toolchain routed most warnings through #pragma warning. The
// modern GNU toolchain relies on diagnostic push/pop helpers, so we provide
// lightweight wrappers here to keep the legacy call sites readable without any
// compiler-specific branching.
#define CNC_WATCOM_PRAGMA(msg) _Pragma(#msg)
#define CNC_WATCOM_DIAGNOSTIC_PUSH _Pragma("GCC diagnostic push")
#define CNC_WATCOM_DIAGNOSTIC_POP _Pragma("GCC diagnostic pop")
#define CNC_WATCOM_IGNORE_UNUSED _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"")

#ifndef CNC_WATCOM_UNUSED
#define CNC_WATCOM_UNUSED(param) ((void)(param))
#endif
