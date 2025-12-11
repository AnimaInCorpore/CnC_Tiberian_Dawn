# Next steps to port the project
Tackle one chunk at a time; when a chunk has no remaining next steps, mark it with "Implementation done!".

## Build system and source layout
Status: Next steps. Keep migrating legacy `.CPP/.H/.ASM` into `src/` with lowercase names, modern includes, and removal of Watcom pragmas/segmented keywords. Expand `CMakeLists.txt` as files move, ensure unified compile flags, and prune obsolete makefiles/ide data once coverage is complete.

## Platform abstraction with SDL
Status: Next steps. Replace DirectDraw/DirectSound/DirectInput/IPX/Greenleaf hooks with SDL2 (render/audio/events) and SDL_net or sockets, preserving timing and event semantics from the Win95 build. Centralize platform shims so gameplay code stays portable.

## Startup and main loop parity
Status: Next steps. Port `STARTUP.CPP`/`CONQUER.CPP` init and teardown into `src/port_stubs.cpp`/`src/game.cpp` (heap sizing, mix/font preloads, intro handling, CD/MMX probes, mouse/setup checks, DDE/network teardown) and swap in the full `MENUS.CPP` flow for main menu, save/load, options, and timeout gating.

## Rendering and UI
Status: Next steps. Complete `DisplayClass::Draw_It/AI` and map helpers in `src/display.cpp`, flesh out `GScreenClass` in `src/gscreen.cpp`, and replace `MapStubClass`/`MapClass` stand-ins (`src/map_shim.cpp`, `src/gameplay_core_stub.cpp`) so rendering, cursor, and radar match Win95. Rebuild HUD/UI pieces (Sidebar/Tab/Radio/Theme, startup options) in `src/gameplay_class_stubs.cpp` and related UI files.

## Audio and messaging
Status: Next steps. Wire `CCMessageBox::Process` and sound entry points in `src/linker_stubs.cpp`; expand `src/audio_stub.cpp` to mix/stream like `AUDIO.CPP` with SDL, mirroring volume, priority, and voice-stealing rules from the original.

## Networking and multiplayer
Status: Next steps. Port IPX/Greenleaf layers (`IPX*.CPP`, `TCPIP.CPP`, `COMBUF/COMQUEUE`) to SDL_net/standard sockets while keeping packet formats and session logic identical. Reattach lobby/setup dialogs and ensure determinism checks and timeouts match the Win95 behavior.

## Data loading and file I/O
Status: Next steps. Modernize MIX/raw file handling (`MIXFILE.CPP`, `RAWFILE.CPP`, `CCFILE.CPP`) for cross-platform paths and endianness, preserving lookup order and caching rules. Bring over config/profile parsing so startup options and saved games behave identically.

## Gameplay systems and AI
Status: Next steps. Incrementally port core gameplay units/structures (`UNIT/TECHNO/BUILDING/INFANTRY/VEHICLE` etc.), pathfinding (`FINDPATH`, `VECTOR`), triggers/scripts, and mission flow, ensuring timers, random seeds, and AI decisions remain deterministic relative to the Win95 reference.

## Assembly replacements
Status: Next steps. Re-implement `.ASM` helpers (MMX, page fault handlers, IPX real-mode glue, text blits) in portable C/C++ with clear comments; remove inline assembly and legacy pragmas while matching byte-exact behavior where required.

## Testing and parity verification
Status: Next steps. Keep a standing build against `g++`/CMake with SDL dependencies, run the Win95 build for side-by-side checks, and add focused harnesses for rendering/audio/input timing regressions. Update `PROGRESS.md` and this file per chunk completion, flipping the chunk marker to "Implementation done!" when finished.
