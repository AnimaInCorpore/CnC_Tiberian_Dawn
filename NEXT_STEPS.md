# Next steps to port the project
Tackle one chunk at a time; when a chunk has no remaining next steps, mark it with "Implementation done!".

## Remove shims/stubs/fallbacks (parity blockers)
Status: Next steps. Scope: remove all portability shims, linker stubs, and "fallback" behaviors and replace them with behavior-complete ports that match the Win95 C&C95 reference; success means the build no longer depends on `src/*stub*.cpp`, `src/*shim*.cpp`, or `src/include/legacy/*_stub.h`, and the UI/render/audio/net paths no longer contain non-canonical fallback code paths.
Implementation done!: Replaced the dialog/background rendering stub by porting `CC_Texture_Fill` and wiring it into `Draw_Box` so green UI panels use `BTEXTURE.SHP` (no more solid-color fills in menus/dialogs).
Implementation done!: Removed the last stub-only gameplay shim translation unit (`src/gameplay_shims.cpp`) and replaced `Choose_Side()` with a working side selection dialog (GDI/Nod) in `src/intro_port.cpp`.
Remove the title-art fallback scan in `src/load_title.cpp` and restore the Win95 asset resolution/order (missing `HTITLE.PCX` should surface as an error, not silently pick another PCX/CPS).
Replace `MapStubClass` (`src/map_shim.cpp`, `src/include/legacy/map_shim.h`) with the real map implementation so all `Map.*` calls have canonical behavior (cell lookups, redraw flags, radar, cursor, object overlap, tactical map projection).
Replace `src/movie.cpp` with real VQA playback (video timing, palette updates, input skip rules) so intro/cutscenes match Win95.
Finish the SDL audio port by matching Win95 mixer behavior (pan law, priority/channel reservation, fades/stops, music/theme streaming) and retire the remaining “stub” naming once parity is reached.
Audit remaining stub-only units in the build (`src/movie.cpp` and any `src/*stub*.cpp` reintroduced) and either port their backing modules or remove the fallback code paths so the build fails loudly when functionality is missing.
Remove palette/animation “fallback” code paths (e.g., `src/interpal_fallback.cpp`) and match the Win95 palette interpolation/animation timing exactly instead of approximating when data is missing.
Remove any remaining Win95-only `WinTimerClass` usage; the port should rely on `TimerClass`/`CountDownTimerClass` (`TickCount`, `ProcessTimer`, `FrameTimer`) for deterministic timing.
Replace `src/include/legacy/getcd.h` placeholder with a behavior-complete port (or remove the code paths if they are truly Win95-only and replaced by canonical SDL equivalents).
Replace `SurfaceCollectionStub` in `src/include/legacy/wwlib32.h` with a real SDL-backed surface/restore model (or an equivalent always-valid implementation) that matches Win95 lost-surface/restore semantics without silently masking failures.
Remove remaining `src/port_runtime.cpp` placeholders that return defaults/no-ops (version/config/profile stubs, CD probing, setup/profile persistence) by porting the original implementations into the SDL runtime layer.
Implementation done!: Removed leftover linker-only stub translation units (`src/gameplay_class_stubs.cpp`, `src/linker_stubs.cpp`, `src/pointer_stubs.cpp`, `src/linker_small.cpp`) and kept `CMakeLists.txt` free of duplicate-symbol fallbacks.

## Build system and source layout
Status: Next steps. Scope: move legacy sources into `src/` with lowercase names, fix includes, strip Watcom/segmented keywords, and keep `CMakeLists.txt` in sync. Uppercase `src` filenames have been normalized; continue migrating the remaining legacy files. Excludes gameplay/runtime changes.
Audit `TD_ENABLE_WERROR` suppression list in `CMakeLists.txt` and retire `-Wno-*` entries by fixing the underlying warnings as modules are fully ported (keep CI strictness meaningful).
Keep `CMakeLists.txt` free of stub-only translation units once real ports exist; prefer failing the build over silently linking fallbacks.
Confirm there are no duplicate-symbol “safety net” objects (old linker shims, gameplay minimal stubs, pointer stubs) and remove the files once they are no longer referenced.
Finish porting the remaining legacy helpers for link parity (full `CONQUER.CPP` keyboard/message handling, `LOADDLG.CPP` save/load dialog, and modem reconnect flows) so the current skeletons can be replaced with behavior-complete implementations.

## Platform abstraction with SDL
Status: Next steps. Scope: create SDL2/SDL_net shims for video/audio/input/network only; replace DirectDraw/DirectSound/DirectInput/IPX/Greenleaf entry points while keeping call signatures so upper layers stay untouched.

## Startup and main loop parity
Status: Next steps. Scope: port `STARTUP.CPP`/`CONQUER.CPP` sequencing into `src/port_runtime.cpp`/`src/game.cpp` (heap sizing, mix/font preload, intro gating, CD/MMX probes, mouse/setup checks, DDE/network teardown) and wire the restored `src/menus.cpp` selections into the real flows (expansion/bonus dialogs, load/intro handling) instead of placeholder GameToPlay switches. Exercise the Special dialog flow end-to-end (checkbox toggles commit and return to the map) now that the stub hooks are present. Excludes rendering/audio/UI internals.
Finish the remaining menu branches in `src/port_runtime.cpp` (expansion/bonus/load/multiplayer/intro) now that start-new-game config is wired to the main menu.

## Rendering and UI
Status: Next steps. Scope: finish `DisplayClass::Draw_It/AI`, map helpers in `src/display.cpp`, `GScreenClass` in `src/gscreen.cpp`, and `MapStubClass`/`MapClass` replacements (`src/map_shim.cpp`, `src/gameplay_core_stub.cpp`), plus HUD/UI widgets (Sidebar/Tab/Radio/Theme, startup options). Hook the ported credit tab (`src/credits.cpp`) back into Sidebar/Tab update loops when those UI classes move over. Excludes audio or net hooks.
Port the Win95 textured UI fill path (`CC_Texture_Fill` + `BTEXTURE.SHP`) and remove the current solid-fill `Draw_Box` implementation so all dialog/menu panels match Win95 visuals.
Verify the restored TXTPRNT ColorXlat-style nibble translation keeps gradient/LED fonts aligned and shaded like the Win95 renderer now that glyph offsets decode correctly.
Re-check 6pt/8pt text spacing after mirroring the legacy Simple_Text_Print x/y offsets so SDL text baselines/advances line up with the Win95 menus.
Verify the nearest-neighbor SDL scale mode on present/title textures keeps text/UI pixels crisp across macOS/Windows output scaling.
Spot-check menu button text after centering tweak in `src/textbtn.cpp` to ensure vertical/horizontal alignment matches the Win95 layout.
Decide whether to surface the `MonoClass` debug buffer (ported in `src/monoc.cpp`) via an SDL overlay or log sink for modern builds.
Implementation done!: Implemented the `WWMouseClass` draw/erase overlay (cursor blit + restore) so the SDL render path shows the legacy cursor now that `GScreenClass::Blit_Display` uses it.
Implementation done!: Replaced wwlib runtime stubs for video mode switching, cursor clipping, and surface restore tracking so SDL builds can correctly constrain the cursor and respond to focus loss.
Compare the main menu title background palette against Win95 output now that `src/load_title.cpp` no longer forces the first 16 UI colors.
Confirm sidebar/radar button input wiring still routes through `GScreenClass::Buttons` once the real `MapClass` replaces the shim.
Replace the palette interpolation fallback with the Win95-equivalent interpolation path and validate output/timing (especially 2x scaled animations) against Win95 captures.
Wire SDL window focus events to `AllSurfaces.Set_Surface_Focus(...)` so dialogs that watch `AllSurfaces.SurfacesRestored` behave like Win95 when the window is deactivated/reactivated.

## Audio and messaging
Status: Next steps. Scope: finish the SDL audio port by validating SFX/music parity against Win95 captures (volume law, pan, priority/reservation, fades/stops) and removing remaining placeholder theme/music behaviors.
Hook `OptionsClass::Set_Score_Volume` into the eventual SDL music/stream volume path so menu volume sliders affect active theme playback.
Port `THEME.CPP` into `src/theme.cpp` so `ThemeClass::Queue_Song`/`Play_Song`/`AI` drive the real music flow instead of stubbed behavior.

## Networking and multiplayer
Status: Next steps. Scope: port IPX/Greenleaf stack (`IPX*.CPP`, `TCPIP.CPP`, `COMBUF/COMQUEUE`) onto SDL_net/standard sockets, preserving packet formats, session/lobby dialogs, determinism checks, and timeout behavior. Excludes local audio/render work.
Confirm the portable CCDDE replacement (`src/ccdde.cpp` + `src/dde.cpp`) matches the Win95 WChat expectations (packet framing, heartbeats, start-game payload) and document the external helper port/env vars (`TD_DDE_PORT`, `TD_DDE_LISTEN_PORT`) for tooling integration.
Port `PACKET.CPP` so PacketClass serialization uses the new `src/field.cpp` implementation for host/net conversions.
Confirm `EventClass::Execute` is wired into the network event dispatch path once the IPX/COMMS layers are ported (timing/response events should still adjust `MPlayerMaxAhead`).

## Data loading and file I/O
Status: Next steps. Scope: modernize MIX/raw/CC file handling (`MIXFILE.CPP`, `RAWFILE.CPP`, `CCFILE.CPP`) for cross-platform paths/endianness, and config/profile parsing for startup/save/load parity. Excludes gameplay logic changes.
Add a sanity check/log when required MIX archives (e.g. `GENERAL.MIX`/`CONQUER.MIX`) are missing now that text rendering depends on the CD font assets instead of stub glyphs.
Add a debug assert around font header size in the SDL text path so regressions in packed parsing surface clearly now that we refuse to draw without valid font data.
Validate the icon-set map offsets used by `Get_Icon_Set_Map` against real template assets and finish the scaling math in `GraphicViewPortClass::Scale` once the renderer is hooked up.

## Gameplay systems and AI
Status: Next steps. Scope: port unit/structure/AI systems (`UNIT/TECHNO/BUILDING/INFANTRY/VEHICLE`, pathfinding `FINDPATH`/`VECTOR`, triggers/scripts, mission flow) ensuring deterministic timers/random seeds. Excludes platform or asset I/O changes. Cargo attach/detach/pointer coding now live in `src/cargo.cpp`; next tie it back into Foot/Unit load/unload once those files move. `src/bullet.cpp` now carries the projectile logic with the data tables in `src/bdata.cpp`; with `src/anim.cpp` ported, hook Foot/Unit/Building callers back into the real animation spawn/attach paths.
Confirm factory production state (from `src/factory.cpp`) is reflected in the Sidebar/Tab UI once those classes are ported so build progress and completion behave like Win95.
Validate the `FlasherClass` updates flow through Techno/Unit redraw paths once those classes are fully ported so flashing feedback matches Win95.
Re-check turret/unit rotation redraw cadence when porting Techno/Unit so the now-restored FacingClass 1/32-zone transition matches Win95 visuals.

## Assembly replacements
Status: Next steps. Scope: re-implement `.ASM` helpers (MMX routines, page fault glue, IPX real-mode bridges, text blits) in portable C/C++ with comments; remove inline assembly/pragmas. Excludes higher-level logic.

## Testing and parity verification
Status: Next steps. Scope: maintain g++/CMake builds with SDL deps, run Win95 side-by-side checks, and add focused harnesses for rendering/audio/input/net timing regressions. Keep `PROGRESS.md` and this file updated; flip a chunk to "Implementation done!" when it is complete.
