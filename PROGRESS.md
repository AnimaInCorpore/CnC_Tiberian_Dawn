# Porting Progress

The table below records every legacy source/header that already has a modern counterpart under `src/`. Update it whenever new files are migrated so the porting backlog stays visible.

| Legacy file | Modern path | Notes |
| --- | --- | --- |
| `FTIMER.H` | `src/FTIMER.h` | Countdown timer helper rewritten with `#pragma once` and the global `Frame` counter. |
| `RAND.H` | `src/RAND.h` | Random helper declarations cleaned up to use `<cstdint>` types. |
| `RAND.CPP` | `src/RAND.cpp` | Random lookup table logic now relies on standard headers and explicit scaling. |
| `VECTOR.H` | `src/VECTOR.h` | Vector template modernized with RAII allocation and portable guards. |
| `VECTOR.CPP` | `src/VECTOR.cpp` | Boolean vector utilities rewritten around clear helpers and `std::memcpy`. |
| `ABSTRACT.H` | `src/include/legacy/abstract.h` | Lowercase mirror retained so `#include "abstract.h"` works on case-sensitive hosts. |
| `COMPAT.H` | `src/include/legacy/compat.h` | Palette/buffer macros and legacy globals wrapped in portable defaults. |
| `DEFINES.H` | `src/include/legacy/defines.h` | Lowercase mirror preserving gameplay feature toggles until modernization. |
| `EXTERNS.H` | `src/include/legacy/externs.h` | Lowercase copy to keep the sprawling extern declarations accessible. |
| `FUNCTION.H` | `src/include/legacy/function.h` | Lowercase mirror ensuring the UI hierarchy declarations keep compiling. |
| `MISSION.H` | `src/include/legacy/mission.h` | Lowercase copy of the mission AI declarations for case-sensitive builds. |
| `OBJECT.H` | `src/include/legacy/object.h` | Lowercase mirror safeguarding the core object hierarchy headers. |
| `REAL.H` | `src/include/legacy/real.h` | Lowercase copy retaining the original fixed-point math helpers. |
| `TARGET.H` | `src/include/legacy/target.h` | Lowercase mirror for the targeting helper declarations. |
| `TYPE.H` | `src/include/legacy/type.h` | Lowercase copy of the shared enums/typedefs used throughout the game. |
| `WWFILE.H` | `src/include/legacy/wwfile.h` | FileClass interface refreshed with `std::size_t` and a namespace alias. |
| `WATCOM.H` | `src/include/legacy/watcom.h` | Watcom pragma wrappers swapped for GCC diagnostic helpers. |
| `PLATFORM (new)` | `src/include/legacy/platform.h` | Win16/Watcom typedef shim that turns `near`/`far` keywords into no-ops. |
| `WINDOWS_COMPAT (new)` | `src/include/legacy/windows_compat.h` | Win32 handle/struct typedef shim so the port never includes platform headers directly. |
