# Next steps to port the project
Tackle one chunk at a time; when a chunk has no remaining next steps, mark it with "Implementation done!".

## Build system and source layout
Status: Next steps. Scope: move legacy sources into `src/` with lowercase names, fix includes, strip Watcom/segmented keywords, and keep `CMakeLists.txt` in sync. Uppercase `src` filenames have been normalized; continue migrating the remaining legacy files. Excludes gameplay/runtime changes.

## Platform abstraction with SDL
Status: Next steps. Scope: create SDL2/SDL_net shims for video/audio/input/network only; replace DirectDraw/DirectSound/DirectInput/IPX/Greenleaf entry points while keeping call signatures so upper layers stay untouched.

## Startup and main loop parity
Status: Next steps. Scope: port `STARTUP.CPP`/`CONQUER.CPP` sequencing into `src/port_stubs.cpp`/`src/game.cpp` (heap sizing, mix/font preload, intro gating, CD/MMX probes, mouse/setup checks, DDE/network teardown) and drop in full `MENUS.CPP` flow. Wire the new `Load_Title_Screen` path into that bootstrap once menus/intro sequencing lands. Excludes rendering/audio/UI internals.

## Rendering and UI
Status: Next steps. Scope: finish `DisplayClass::Draw_It/AI`, map helpers in `src/display.cpp`, `GScreenClass` in `src/gscreen.cpp`, and `MapStubClass`/`MapClass` replacements (`src/map_shim.cpp`, `src/gameplay_core_stub.cpp`), plus HUD/UI widgets (Sidebar/Tab/Radio/Theme, startup options). Hook the ported credit tab (`src/credits.cpp`) back into Sidebar/Tab update loops when those UI classes move over. Excludes audio or net hooks.

## Audio and messaging
Status: Next steps. Scope: wire `CCMessageBox::Process` and audio entry points in `src/linker_stubs.cpp`; rebuild `src/audio_stub.cpp` to match `AUDIO.CPP` mixing/streaming via SDL with original volume/priority/voice rules. Excludes rendering or net.

## Networking and multiplayer
Status: Next steps. Scope: port IPX/Greenleaf stack (`IPX*.CPP`, `TCPIP.CPP`, `COMBUF/COMQUEUE`) onto SDL_net/standard sockets, preserving packet formats, session/lobby dialogs, determinism checks, and timeout behavior. Excludes local audio/render work.

## Data loading and file I/O
Status: Next steps. Scope: modernize MIX/raw/CC file handling (`MIXFILE.CPP`, `RAWFILE.CPP`, `CCFILE.CPP`) for cross-platform paths/endianness, and config/profile parsing for startup/save/load parity. Excludes gameplay logic changes.

## Gameplay systems and AI
Status: Next steps. Scope: port unit/structure/AI systems (`UNIT/TECHNO/BUILDING/INFANTRY/VEHICLE`, pathfinding `FINDPATH`/`VECTOR`, triggers/scripts, mission flow) ensuring deterministic timers/random seeds. Excludes platform or asset I/O changes. Cargo attach/detach/pointer coding now live in `src/cargo.cpp`; next tie it back into Foot/Unit load/unload once those files move. `src/bullet.cpp` now carries the projectile logic; follow up by moving the bullet data tables (`BDATA.CPP`) and the animations it references (`ANIM.CPP`) so the type definitions and explosion visuals line up.

## Assembly replacements
Status: Next steps. Scope: re-implement `.ASM` helpers (MMX routines, page fault glue, IPX real-mode bridges, text blits) in portable C/C++ with comments; remove inline assembly/pragmas. Excludes higher-level logic.

## Testing and parity verification
Status: Next steps. Scope: maintain g++/CMake builds with SDL deps, run Win95 side-by-side checks, and add focused harnesses for rendering/audio/input/net timing regressions. Keep `PROGRESS.md` and this file updated; flip a chunk to "Implementation done!" when it is complete.
