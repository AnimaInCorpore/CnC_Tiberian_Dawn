#pragma once

#include <cstdint>

#include "platform.h"

// The modern port mirrors the original Win32 release. Even when building on
// non-Windows platforms we provide compatible typedefs so legacy source code
// that expects Win32 semantics still compiles. We always use the lightweight
// shims below so the build never depends on platform-specific headers.
using HWND = void*;
using HANDLE = void*;
using HINSTANCE = void*;
using HMODULE = void*;
using HDC = void*;
using HBRUSH = void*;
using HCURSOR = void*;
using HICON = void*;
using HFONT = void*;
using HMENU = void*;
using HGDIOBJ = void*;
using HGLOBAL = void*;
using HBITMAP = void*;
using WPARAM = std::uintptr_t;
using LPARAM = std::intptr_t;
using LRESULT = std::intptr_t;
using ATOM = unsigned short;
using COLORREF = std::uint32_t;
using LCID = std::uint32_t;
using LANGID = std::uint16_t;
using WORD = std::uint16_t;
using DWORD = std::uint32_t;
using BYTE = std::uint8_t;

using LPVOID = void*;
using LPCVOID = void const*;
using LPSTR = char*;
using LPCSTR = char const*;
using LPTSTR = char*;
using LPCTSTR = char const*;
using LPOVERLAPPED = void*;
using LPSECURITY_ATTRIBUTES = void*;
using FARPROC = void (*)();

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#ifndef CALLBACK
#define CALLBACK
#endif
#ifndef WINAPI
#define WINAPI
#endif
#ifndef APIENTRY
#define APIENTRY
#endif
#ifndef PASCAL
#define PASCAL
#endif
#ifndef pascal
#define pascal
#endif
#ifndef FAR
#define FAR
#endif
#ifndef NEAR
#define NEAR
#endif
#ifndef EXPORT
#define EXPORT
#endif
#ifndef _export
#define _export
#endif
#ifndef _stdcall
#define _stdcall
#endif
#ifndef __stdcall
#define __stdcall
#endif
#ifndef _fastcall
#define _fastcall
#endif
#ifndef __fastcall
#define __fastcall
#endif

#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE reinterpret_cast<HANDLE>(-1)
#endif

using DLGPROC = BOOL (*)(HWND, UINT, WPARAM, LPARAM);

struct POINT {
  long x;
  long y;
};

struct RECT {
  long left;
  long top;
  long right;
  long bottom;
};

#ifndef LOWORD
#define LOWORD(l) (static_cast<unsigned>(reinterpret_cast<std::uintptr_t>(l)) & 0xFFFF)
#endif
#ifndef HIWORD
#define HIWORD(l) ((static_cast<unsigned>(reinterpret_cast<std::uintptr_t>(l)) >> 16) & 0xFFFF)
#endif
#ifndef MAKELONG
#define MAKELONG(low, high) ((static_cast<std::uint32_t>(high) << 16) | ((low) & 0xFFFF))
#endif
