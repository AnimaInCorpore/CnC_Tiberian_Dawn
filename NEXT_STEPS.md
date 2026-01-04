# Next Steps

Prioritized follow-up tasks to continue porting the legacy C&C95 Win32 sources into a modern, cross-platform (SDL2) build while preserving original behavior.

## 1) Close the remaining legacy `.CPP` gaps (9 files)

Create the missing `src/<lower>.cpp` translation units (or explicitly retire them if obsolete) and keep behavior aligned with the Win32 reference:

- [x] `CONQUER.CPP` → `src/conquer.cpp`
- [x] `GADGET.CPP` → `src/gadget.cpp`
- [x] `INIT.CPP` → `src/init.cpp`
- [x] `KEYFRAME.CPP` → `src/keyframe.cpp`
- [ ] `MAPEDDLG.CPP` → `src/mapeddlg.cpp`
- [ ] `MAPEDPLC.CPP` → `src/mapedplc.cpp`
- [ ] `MAPEDTM.CPP` → `src/mapedtm.cpp`
- [x] `MPLAYER.CPP` → `src/mplayer.cpp` (present, not in CMake target yet; depends on `Select_Serial_Dialog`/modem UI work)
- [ ] `NETDLG.CPP` → `src/netdlg.cpp`
- [ ] `NULLDLG.CPP` → `src/nulldlg.cpp`
- [ ] `STARTUP.CPP` → `src/startup.cpp` (entry/init logic currently spread across `src/main.cpp`/`src/game.cpp`)
- [ ] `TEMP.CPP` — retire (legacy file appears truncated/invalid; no references in the modern build)
- [ ] `WINSTUB.CPP` → `src/winstub.cpp` (likely stays retired; confirm Windows launcher approach)

## 2) Resolve the remaining legacy `.ASM` (4 pending + 1 Win32-specific)

- [ ] `KEYFBUFF.ASM` — port to C/C++ (or replace by reworking the callers to use the modern blitters)
- [ ] `PAGFAULT.ASM` — determine if still required; if so, port to portable memory/page-fault-safe logic
- [ ] `SUPPORT.ASM` — port/replace any remaining helper routines still referenced from C++
- [ ] `WINASM.ASM` — confirm whether anything still depends on it; retire or replace with portable code (or Win32-only C++)

(`IPXPROT.ASM`/`IPXREAL.ASM` are considered retired for the flat 32/64-bit port; see `PROGRESS.md`.)

## 3) Wire up “ported but not built” sources

Currently present in `src/` but not compiled by the main `cnc_tiberian_dawn` CMake target:

- [ ] Decide whether to include and maintain `src/mapedit.cpp`, `src/mapedsel.cpp`, and `src/mapsel.cpp` (map editor/UI selection flow)
- [ ] Either integrate or retire helper-only units (`src/ini_helpers.cpp`, `src/scenario_helpers.cpp`)

## 4) Verification cadence (keep behavior canonical)

- [ ] After each major module lands, compare flow/behavior vs. the Win32 build (boot → title → menus → in-game loop)
- [ ] Add small regression tests under `src/tests/` when a subsystem is stabilized (e.g., file/CRC helpers, PCX/CPS decode, UI widget behavior)
