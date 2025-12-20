# Next steps to port the project
Tackle one chunk at a time; when a chunk has no remaining next steps, mark it with "Implementation done!".

## Build system and source layout
Status: Next steps. Scope: move legacy sources into `src/` with lowercase names, fix includes, strip Watcom/segmented keywords, and keep `CMakeLists.txt` in sync. Uppercase `src` filenames have been normalized; continue migrating the remaining legacy files. Excludes gameplay/runtime changes.

## Platform abstraction with SDL
Status: Next steps. Scope: create SDL2/SDL_net shims for video/audio/input/network only; replace DirectDraw/DirectSound/DirectInput/IPX/Greenleaf entry points while keeping call signatures so upper layers stay untouched.

## Startup and main loop parity
Status: Next steps. Scope: port `STARTUP.CPP`/`CONQUER.CPP` sequencing into `src/port_stubs.cpp`/`src/game.cpp` (heap sizing, mix/font preload, intro gating, CD/MMX probes, mouse/setup checks, DDE/network teardown) and wire the restored `src/menus.cpp` selections into the real flows (expansion/bonus dialogs, load/intro handling) instead of placeholder GameToPlay switches. Exercise the Special dialog flow end-to-end (checkbox toggles commit and return to the map) now that the stub hooks are present. Excludes rendering/audio/UI internals.

## Rendering and UI
Status: Next steps. Scope: finish `DisplayClass::Draw_It/AI`, map helpers in `src/display.cpp`, `GScreenClass` in `src/gscreen.cpp`, and `MapStubClass`/`MapClass` replacements (`src/map_shim.cpp`, `src/gameplay_core_stub.cpp`), plus HUD/UI widgets (Sidebar/Tab/Radio/Theme, startup options). Hook the ported credit tab (`src/credits.cpp`) back into Sidebar/Tab update loops when those UI classes move over. Replace the remaining shape draw path (`CC_Draw_Shape` + `Build_Frame` LCW decode) so render callers hit real blits. Excludes audio or net hooks.
Verify the restored TXTPRNT ColorXlat-style nibble translation keeps gradient/LED fonts aligned and shaded like the Win95 renderer now that glyph offsets decode correctly.
Re-check 6pt/8pt text spacing after mirroring the legacy Simple_Text_Print x/y offsets so SDL text baselines/advances line up with the Win95 menus.
Verify the nearest-neighbor SDL scale mode on present/title textures keeps text/UI pixels crisp across macOS/Windows output scaling.
Spot-check menu button text after centering tweak in `src/textbtn.cpp` to ensure vertical/horizontal alignment matches the Win95 layout.
Decide whether to surface the `MonoClass` debug buffer (ported in `src/monoc.cpp`) via an SDL overlay or log sink for modern builds.

## Audio and messaging
Status: Next steps. Scope: wire `CCMessageBox::Process` and audio entry points in `src/linker_stubs.cpp`; rebuild `src/audio_stub.cpp` to match `AUDIO.CPP` mixing/streaming via SDL with original volume/priority/voice rules. Excludes rendering or net.

## Networking and multiplayer
Status: Next steps. Scope: port IPX/Greenleaf stack (`IPX*.CPP`, `TCPIP.CPP`, `COMBUF/COMQUEUE`) onto SDL_net/standard sockets, preserving packet formats, session/lobby dialogs, determinism checks, and timeout behavior. Excludes local audio/render work.
Port `PACKET.CPP` so PacketClass serialization uses the new `src/field.cpp` implementation for host/net conversions.
Confirm `EventClass::Execute` is wired into the network event dispatch path once the IPX/COMMS layers are ported (timing/response events should still adjust `MPlayerMaxAhead`).

## Data loading and file I/O
Status: Next steps. Scope: modernize MIX/raw/CC file handling (`MIXFILE.CPP`, `RAWFILE.CPP`, `CCFILE.CPP`) for cross-platform paths/endianness, and config/profile parsing for startup/save/load parity. Excludes gameplay logic changes.
Add a sanity check/log when required MIX archives (e.g. `GENERAL.MIX`/`CONQUER.MIX`) are missing now that text rendering depends on the CD font assets instead of stub glyphs.
Add a debug assert around font header size in the SDL text path so regressions in packed parsing surface clearly now that we refuse to draw without valid font data.

## Gameplay systems and AI
Status: Next steps. Scope: port unit/structure/AI systems (`UNIT/TECHNO/BUILDING/INFANTRY/VEHICLE`, pathfinding `FINDPATH`/`VECTOR`, triggers/scripts, mission flow) ensuring deterministic timers/random seeds. Excludes platform or asset I/O changes. Cargo attach/detach/pointer coding now live in `src/cargo.cpp`; next tie it back into Foot/Unit load/unload once those files move. `src/bullet.cpp` now carries the projectile logic with the data tables in `src/bdata.cpp`; with `src/anim.cpp` ported, hook Foot/Unit/Building callers back into the real animation spawn/attach paths.
Validate the `FlasherClass` updates flow through Techno/Unit redraw paths once those classes are fully ported so flashing feedback matches Win95.
Re-check turret/unit rotation redraw cadence when porting Techno/Unit so the now-restored FacingClass 1/32-zone transition matches Win95 visuals.

## Assembly replacements
Status: Next steps. Scope: re-implement `.ASM` helpers (MMX routines, page fault glue, IPX real-mode bridges, text blits) in portable C/C++ with comments; remove inline assembly/pragmas. Excludes higher-level logic.

## Testing and parity verification
Status: Next steps. Scope: maintain g++/CMake builds with SDL deps, run Win95 side-by-side checks, and add focused harnesses for rendering/audio/input/net timing regressions. Keep `PROGRESS.md` and this file updated; flip a chunk to "Implementation done!" when it is complete.
