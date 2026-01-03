| Legacy file | Modern path | Parity check | Notes |
| --- | --- | --- | --- |
| Parity checks (techno/building/unit/network dialog) | `src/techno.cpp`, `src/building.cpp`, `src/unit.cpp`, `src/network_helpers.cpp` | manual | Restored Win95 behaviors: Techno window X shift now matches legacy precedence, refinery backup scatter uses the legacy zero threat coordinate, unit mono debug dump uses original CP437 box-drawing bytes via escape sequences, and Net_Reconnect_Dialog clears the timeout row before redraw. |
| Parity checks (load/save dialog) | `src/loaddlg.cpp` | manual | Checked: `src/loaddlg.cpp` matches `LOADDLG.CPP` aside from portable filesystem/time handling, safe string/printf updates, and SDL-facing gadget sizing defaults (no behavior change expected in the SDL build). |
| Parity checks (score/map editor headers) | `src/score.cpp`, `src/include/legacy/function.h`, `src/mapedit.cpp` | manual | Checked: `src/score.cpp` matches `SCORE.CPP` aside from safe C++/formatting tweaks; `src/score.cpp` now includes headers via `legacy/...` to avoid case-sensitive include ambiguity. `src/mapedit.cpp` is identical to `MAPEDIT.CPP`, and `XYP_COORD` matches `FUNCTION.H`/`REAL.H`. |
| Parity fix (score volume live update) | `src/options.cpp`, `src/audio_play.cpp`, `src/include/legacy/function.h` | build | Restored Win95 `Set_Score_Vol(...)` behavior: changing the score volume now immediately affects currently playing music by scaling streamed score playback in the mixer. |
| Include path hygiene (large gameplay units) | `src/building.cpp`, `src/unit.cpp`, `src/techno.cpp`, `src/infantry.cpp`, `src/aircraft.cpp`, `src/cdata.cpp` | build | Switched the largest ported translation units from `#include "function.h"` to `#include "legacy/function.h"` to avoid case-insensitive header resolution and ensure deterministic behavior on case-sensitive toolchains. |
| `MAP.CPP` | `src/map.cpp`, `src/include/legacy/defines.h` | manual | Restored Win95 tile BIN load semantics by using an 8-bit `TemplateType` and the original read path, so template/icon pairs are parsed exactly like the legacy build. |
| `MAPEDIT.CPP` / `MAPEDSEL.CPP` (editor includes) | `src/mapedit.cpp`, `src/mapedsel.cpp` | build | Updated editor-only translation units to include `legacy/function.h` so case-sensitive builds don’t depend on `function.h` being present in the include path. |
| `JSHELL.CPP` | `src/jshell.cpp` | differs | Ported `Small_Icon()` by parsing the existing iconset buffer layout (same offsets already used by `GraphicViewPortClass::Draw_Stamp`) so radar mini-icon generation matches the Win95 sampling pattern (removed the non-legacy 0xFF map guard). |
| `CCFILE.CPP` | `src/ccfile.cpp` | differs | CCFileClass now matches Win95 MIX semantics: prefer loose/on-disk overrides when present, otherwise open embedded MIX members from cached RAM images or by seeking within the on-disk parent MIX while keeping the embedded filename bound to the object. |
| `GETCD` (Win95 CD probe) | `src/include/legacy/getcd.h`, `src/getcd.cpp` | manual | Replaced the dummy CD enumerator with a portable implementation that models "CD drives" as repo-local `CD/...` disc mirror roots, keeping the original `Get_First_CD_Drive`/`Get_Next_CD_Drive` call pattern intact. |
| `CONQUER.CPP` (`Force_CD_Available`) | `src/port_runtime.cpp`, `src/game.cpp` | manual | `Force_CD_Available(cd)` now validates the required disc mirror roots (GDI/NOD/COVERT) and prompts to retry/cancel when missing; startup MIX registration now registers all matching archives across the known `CD/...` mirrors so disc-specific assets (e.g., movies) resolve correctly. |
| `INIT.CPP` (`Select_Game`/attract/menu defaults) | `src/port_runtime.cpp`, `src/menus.cpp`, `src/recording.cpp` | manual | Main menu now mirrors Win95 behavior for "return to multiplayer" by pre-selecting the multiplayer entry when the previous game was not `GAME_NORMAL`; the Internet/WChat handoff path (`Do_The_Internet_Menu_Thang` + `Check_From_WChat`) now routes into the multiplayer flow in `GAME_INTERNET` mode; menu timeouts enter attract-mode playback by loading `RECORD.BIN` via `Load_Recording_Values()` when `AllowAttract` is enabled. |
| `INIT.CPP` (`Main_Menu` enter key selection) | `src/menus.cpp` | manual | `KN_RETURN` now returns the same selection codes as clicking a button (based on button IDs), matching Win95 menu behavior in all menu variants. |
| `SOUNDDLG.CPP` / `VISUDLG.CPP` | `src/sounddlg.cpp`, `src/visudlg.cpp` | manual | Implemented the Sound/Visual Controls dialogs (track list + play/stop, shuffle/repeat, music/SFX sliders; brightness/color/contrast/tint sliders with reset) so the in-game options path no longer calls no-op handlers. |
| `RAWFILE.CPP` | `src/rawfile.cpp` | differs | Restored Win95 file semantics: `Read()`/`Write()` now auto-open/close the file when needed (so call sites like `CCFileClass(...).Write(...)` work), `Seek()` again treats unopened handles as fatal, `Open()` now retries like Win95 (retry on `ENOENT` / hard errors via `Error()`), and `Is_Available(forced)` honors forced-open behavior. |
| `RAWFILE.CPP` (write/close error parity) | `src/rawfile.cpp` | manual | Writes now `fsync()`/`_commit()` on close when opened with write rights and surface commit/close failures via `RawFileClass::Error()`, improving “config writes hit disk and show errors” parity with Win95. |
| `TEXT.CPP` | `src/text.cpp` | legacy missing | Fixed 4bpp font glyph decoding: the high nibble is now shifted before `ColorXlat` lookup so text no longer renders with corrupted colors/holes. |
| `GLOBALS.CPP` | `src/globals.cpp` | differs | Ported to src/, replaced NULL with nullptr, disabled legacy networking code, and restored the `Hard_Error_Occured` global used by legacy file I/O. |
| `FTIMER.H` | `src/include/ftimer.h` | differs | Countdown timer helper rewritten with `#pragma once` and the global `Frame` counter. |
| `RAND.H` | `src/rand.h` | legacy missing | Random helper declarations cleaned up to use `<cstdint>` types. |
| `RAND.CPP` | `src/rand.cpp` | differs | Random lookup table logic now relies on standard headers and explicit scaling. |
| `VECTOR.H` | `src/vector.h` | differs | Vector template modernized with RAII allocation and portable guards. |
| `VECTOR.CPP` | `src/vector.cpp` | differs | Boolean vector utilities rewritten around clear helpers and `std::memcpy`. |
| `LINK.CPP` | `src/link.cpp` | differs | Doubly linked helper now uses `nullptr` checks and RAII-friendly removal. |
| `LAYER.CPP` | `src/layer.cpp` | differs | Layer manager handles sorting; load/save and pointer coding live in `src/ioobj.cpp`. |
| `LIST.CPP` | `src/list.cpp` | differs | List gadget clamps indices, wires the scroll peers, and manages slider state in pure C++. |
| `RADIO.CPP` | `src/radio.cpp` | differs | Radio messaging logic wrapped in portable helpers with `nullptr`-safe contact handling. |
| `TAB.CPP` | `src/tab.cpp` | differs | Sidebar tab UI now lives in `src/` with `nullptr` guards and cleaned text drawing helpers. |
| `TOGGLE.CPP` | `src/toggle.cpp` | differs | Windows-style toggle gadget refactored with explicit hover handling and sticky state flow. |
| `TXTLABEL.CPP` | `src/txtlabel.cpp` | differs | Text label gadget retained the quirky format print path with optional clipping width tracking. |
| `CHECKBOX.CPP` | `src/checkbox.cpp` | differs | Checkbox gadget draws the interior using the shared box helpers and keeps mouse flicker hidden. |
| `CHEKLIST.CPP` | `src/cheklist.cpp` | differs | Check list delegates selection to `ListClass` then toggles the inline checkmark unless read-only. |
| `COLRLIST.CPP` | `src/colrlist.cpp` | differs | Color-aware list tracks a parallel color vector and draws selected entries using the requested style. |
| `EDIT.CPP` | `src/edit.cpp` | differs | Edit control rebuilt with explicit focus handling, read-only guards, and caret drawing driven by the modern viewport helpers. |
| `GAUGE.CPP` | `src/gauge.cpp` | differs | Gauge widgets now render their fill/tri-color bars via the modern viewport helpers and honor sticky mouse drags. |
| `SHAPEBTN.CPP` | `src/shapebtn.cpp` | differs | Shape buttons wrap the common `ToggleClass` flow and pick the correct frame for disabled/pressed/reflect modes. |
| `SLIDER.CPP` | `src/slider.cpp` | differs | Slider gadget now owns its plus/minus peers, recalculates thumb geometry, and handles clicks outside the thumb area. |
| `SCROLL.CPP` | `src/scroll.cpp` | differs | Map edge scrolling now clamps the virtual cursor, honors inertia, and keeps the mouse override logic intact. |
| `TEXTBTN.CPP` | `src/textbtn.cpp` | differs | Text buttons resize around the assigned label, draw Windows-style boxes, and respect the green gradient flags. |
| `TEXT.CPP` | `src/text.cpp` | legacy missing | Font loader now registers CD MIX archives before loading fonts and applies the legacy X spacing when measuring/drawing strings. |
| `TEXT.CPP` | `src/text.cpp` | legacy missing | Text rendering now requires the real CD font assets (no built-in fallback), validates header blocks, and reads glyph data via absolute offsets to fix garbled menu text. |
| `AADATA.CPP` | `src/aadata.cpp` | differs | Ported to src/, replaced NULL with nullptr, `stricmp` with `strcasecmp`, `sprintf` and `_makepath` with `std::string` manipulation, and updated include path for `function.h`. |
| `ADATA.CPP` | `src/adata.cpp` | differs | Ported to src/, updated include to the legacy shim, and replaced `_makepath` with `std::string` construction for SHP lookup. |
| `ABSTRACT.CPP` | `src/abstract.cpp` | differs | Abstract base logic now reports the correct RTTI/ownable mask, preserves the building distance fudge, and copies INI names with `std::strncpy`. |
| `AIRCRAFT.CPP` | `src/aircraft.cpp` | differs | Full aircraft gameplay/AI logic moved to src/, restoring the original missions, drawing, and targeting behavior; INI serialization now uses bounded snprintf formatting. |
| `TEXT.CPP` (ColorXlat) | `src/text.cpp` | legacy missing | Glyph draw path now mirrors the legacy ColorXlat translation so gradient/LED fonts use their palette ramps instead of a flat foreground fill. |
| `TXTPRNT.ASM` | `src/txtprnt.cpp` | differs | Replaced assembly `Buffer_Print` and `ColorXlat` with portable implementations; provides `Buffer_Print()` and `Get_Font_Palette_Ptr()`. |
| `TEXT.CPP` (spacing) | `src/text.cpp` | legacy missing | Font X/Y spacing now mirrors the legacy Simple_Text_Print rules per font/shadow flag, fixing misaligned SDL text runs. |
| `TEXTBTN.CPP` | `src/textbtn.cpp` | differs | Button labels now center horizontally/vertically using the active font height so menu text aligns cleanly inside the boxes. |
| `DIALOG.CPP` (`Draw_Box`/`CC_Texture_Fill`) | `src/dialog.cpp` | differs | Restored the Win95 `Draw_Box` renderer (including textured green fills via `BTEXTURE.SHP`) and removed the solid-fill stub implementation. |
| `WWLIB32` (video/mouse/surfaces) | `src/wwlib_runtime.cpp`, `src/include/legacy/wwlib32.h` | manual | Replaced the remaining wwlib runtime stubs with SDL-backed video mode updates, real cursor clipping, and focus/restore tracking via `AllSurfaces`. |
| `ALLOC.CPP` | `src/alloc.cpp` | differs | Legacy allocator collapsed onto the modern malloc-based wrapper (moved from the wwalloc port) while keeping the legacy entry points. |
| `ANIM.CPP` | `src/anim.cpp` | differs | Animation system moved to src/; keeps spawn/attach logic, scorch/crater side effects, and translucent draw path intact with nullptr-safe ownership handling; load/save and pointer coding now come from `src/ioobj.cpp`. |
| `AUDIO.CPP` | `src/audio.cpp` | differs | Ported to src/; EVA/sfx logic retained and playback decision path wired. SDL playback backend lives in `src/audio_play.cpp` (AUD decode + mixer + theme helpers). |
| `BASE.CPP` | `src/base.cpp` | differs | Ported to src/, implements base list parsing, save/load and node lookup. |
| `BBDATA.CPP` | `src/bbdata.cpp` | differs | BulletTypeClass definitions and tables ported; shape loading now uses std::string paths and the modern file helpers. |
| `BDATA.CPP` | `src/bdata.cpp` | differs | Building type tables and helpers ported; loads cameos/buildup shapes via portable paths, initializes real Pointers/placement/repair logic, and sets the WEAP2 overlay hook. |
| `BUILDING.CPP` | `src/building.cpp` | differs | Ported to src/ — full building gameplay/AI/drawing logic implemented. |
| `BULLET.CPP` | `src/bullet.cpp` | differs | Projectile flight/fuse logic ported; keeps homing/arc/drop behaviors, shadow rendering, and explosion damage paths intact. |
| `CARGO.CPP` | `src/cargo.cpp` | differs | Cargo hold bookkeeping ported; attach/detach preserve the chained LIFO order, while pointer coding now relies on `src/ioobj.cpp`. |
| `CCDDE.CPP` | `src/ccdde.cpp` | differs | Ported as a portable localhost UDP implementation for launcher/lobby messaging. |
| `CCFILE.CPP` | `src/ccfile.cpp` | differs | Mix-aware file wrapper now streams embedded MIX entries from disk (no implicit full-MIX caching), matching the legacy “seek into container” behavior and avoiding huge `MOVIES.MIX` allocations. |
| `CDATA.CPP` | `src/cdata.cpp` | identical | Ported to src/; template type tables restored (needs icon-set map helpers and viewport stamp/scale support). |
| `CDFILE.CPP` | `src/cdfile.cpp` | differs | CD/file search helper now preserves search order, restores the original filename before falling back to a direct open, prioritizes the selected mirror drive, and uses the repo-local `CD/...` asset mirror. |
| `GAME.CPP` | `src/game.cpp` | legacy missing | Mix bootstrap registers scenario/theater/audio/movie archives (plus scans `SC*.MIX`) from the repo-local `CD/...` mirrors so missions can load; fixed startup config parsing to free `Load_Alloc_Data` buffers via `Free()` (prevents an invalid `delete[]` abort). |
| `CELL.CPP` | `src/cell.cpp` | differs | Partial port: core helpers implemented (constructor, lookup, redraw). |
| `COMBAT.CPP` | `src/combat.cpp` | differs | Ported: `Modify_Damage` and `Explosion_Damage` implemented. |
| `COMBUF.CPP` | `src/combuf.cpp` | differs | Ported core queue operations and timing helpers. |
| `COMQUEUE.CPP` | `src/comqueue.cpp` | differs | Ported circular send/recv queue (Next/Unqueue/Get semantics) and restored `Mono_Debug_Print2` packet dump output. |
| `CONFDLG.CPP` | `src/confdlg.cpp` | differs | Ported — confirmation dialog implementation with safe string copy. |
| `CONNECT.CPP` | `src/connect.cpp` | differs | Ported (portable implementation; Time uses std::chrono). |
| `CONQUER.CPP` | `src/maingame.cpp` | differs | Main_Game loop ported from the legacy file; retains SDL_QUIT push on exit while routing through the original select/loop/dialog flow, with explicit SpecialDialog cases covered. |
| `CONQUER.CPP` (source helpers) | `src/source_helpers.cpp` | differs | `Source_From_Name`/`Name_From_Source` now follow the legacy lookup table instead of the placeholder mapping. |
| `CONST.CPP` | `src/const.cpp` | differs | Weapon/warhead tables and coordinate helpers brought over with lowercase includes and portable tables. |
| `STARTUP.CPP` (bootstrap) | `src/port_runtime.cpp`, `src/menus.cpp` | manual | Init_Game/Select_Game/Main_Loop now allocate palettes/shape buffers, configure viewports, reset menu state, enable the DDE heartbeat stub, pace the frame loop using the legacy timer defaults, route main-menu choices through the canonical start/expansion/bonus flows, and make `Call_Back()` service SDL events + focus restore + blit/present for fades and modal UI loops; main menu fade-in now uses the title palette and mirrors Win95’s “fade once per entry” behavior. |
| `STARTUP.CPP` (bootstrap) | `src/port_runtime.cpp` | differs | Initialized the global fixed heaps (`Units`, `Buildings`, `TeamTypes`, etc.) via `Set_Heap()` in `Init_Game()` so scenario loads can allocate objects (fixes crash in `TeamTypeClass::Init()` during `Clear_Scenario()`). |
| `STARTUP.CPP` (bootstrap) | `src/port_runtime.cpp` | differs | Added fail-fast asset validation in `Init_Game()` with a clear stderr message when the repo-local `CD/...` asset mirror is missing. |
| `SETUP.CPP` (CONQUER.INI) | `src/port_setup.cpp` | legacy missing | SDL bootstrap now auto-creates a minimal `CONQUER.INI` in the working directory when missing so the port can start from a repo-local `CD/...` mirror without running the legacy Win95 `SETUP.EXE`. |
| `CONTROL.CPP` | `src/control.cpp` | differs | Control gadgets now propagate peer redraws, return KN_BUTTON IDs when triggered, and keep peers wired via a portable nullptr-safe link. |
| `COORD.CPP` | `src/coord.cpp` | differs | Modernized coordinate helpers; `Cardinal_To_Fixed` and `Fixed_To_Cardinal` ported from `COORDA.ASM`. |
| `CREDITS.CPP` | `src/credits.cpp` | differs | Credit counter now ticks toward the player's funds, plays up/down cues, and redraws the tab with resolution scaling. |
| `CREW.CPP` | `src/crew.cpp` | differs | Ported (original translation unit is effectively empty; header-only helpers remain in `crew.h`). |
| `DDE.CPP` | `src/dde.cpp` | differs | Ported as a portable localhost UDP implementation that preserves the legacy API surface. |
| `DEBUG.CPP` | `src/debug.cpp` | differs | Partial port: `Debug_Key` and `Self_Regulate` implemented; kept feature subset for SDL input. |
| `DESCDLG.CPP` | `src/descdlg.cpp` | differs | Ported `DescriptionClass::Process` (dialog UI with edit control and buttons). |
| `DIAL8.CPP` | `src/dial8.cpp` | differs | Ported to src/ (gadget; hides/shows mouse, draws facing dial). |
| `DIALOG.CPP` | `src/dialog.cpp` | differs | Ported: `Dialog_Box`, `Format_Window_String`, `Window_Box`. |
| `DISPLAY.CPP` | `src/display.cpp` | differs | Palette tables rebuilt, fade routines wired, and display scaffolding moved to src/; pointer coding now handled in `src/iomap.cpp`. |
| `DOOR.CPP` | `src/door.cpp` | differs | Ported to src/ (door animation state machine; open/close logic). |
| `DPMI.CPP` | `src/dpmi.cpp` | differs | Ported to src/ with flat-memory `Swap()` implementation (no asm). |
| `DRIVE.CPP` | `src/drive.cpp` | differs | Ported to src/ with full legacy movement logic restored; Map shim call sites use the port’s compatibility layer. |
| `ENDING.CPP` | `src/ending.cpp` | differs | Ported: GDI/NOD ending sequences, movie playback and selection UI. |
| `EVENT.CPP` | `src/event.cpp` | differs | Ported event constructors and execution logic, including mission assignments, production, timing updates, and special handling. |
| `EXPAND.CPP` | `src/expand.cpp` | manual | Ported `Expansion_Present`, `Expansion_Dialog`, and `Bonus_Dialog` so NEWMENU can list and launch expansion (20–59) and bonus (60–62) missions using the same listbox UI and INI `Basic/Name` lookup as Win95 (kept legacy `memcpy` name copy + item selection semantics). |
| `FACING.CPP` | `src/facing.cpp` | differs | Ported to src/ with legacy facing rotation and adjustment logic preserved. |
| `FLASHER.CPP` | `src/flasher.cpp` | differs | Ported to src/; flash countdown toggles the blush flag and exposes mono debug output. |
| `FACTORY.CPP` | `src/factory.cpp` | differs | Ported to src/, switched to legacy include path, and replaced NULL with nullptr. |
| `FIELD.CPP` | `src/field.cpp` | differs | Ported to src/; moved to portable headers and retained original net byte-order conversions. |
| `FINDPATH.CPP` | `src/findpath.cpp` | differs | Ported to src/ with pathfinding and FootClass path helpers restored. |
| `FLY.CPP` | `src/fly.cpp` | differs | Ported FlyClass movement/physics and speed throttle logic; removed `src/fly_stub.cpp`. |
| `FOOT.CPP` | `src/foot.cpp` | differs | Ported to src/ with legacy movement/mission logic intact and includes updated for the SDL build. |
| `FUSE.CPP` | `src/fuse.cpp` | differs | Ported to src/; fuse countdown and explosion trigger logic preserved. |
| `GADGET.CPP` | `src/gadget_control.cpp` | differs | Ported base GadgetClass chain/input logic into a modernized translation unit (same API, SDL-friendly internals). |
| `GAMEDLG.CPP` | `src/gamedlg.cpp` | differs | Ported to src/; game-dialog helpers now compile and run under SDL. |
| `GOPTIONS.CPP` | `src/goptions.cpp` | differs | Ported to src/; options dialog flow restored (needs SDL UI verification). |
| `GSCREEN.CPP` | `src/gscreen.cpp` | differs | Shadow-page setup and render/IO stubs recreated around modern buffers; pointer coding now handled in `src/iomap.cpp`. |
| `HDATA.CPP` | `src/hdata.cpp` | differs | House type table migrated; colors/remap tables kept intact and Jurassic palette tweak guarded behind the Special/AreThingiesEnabled flags. |
| `HEAP.CPP` | `src/heap.cpp` | differs | Ported to src/; heap bookkeeping is now portable. |
| `HELP.CPP` | `src/help.cpp` | differs | Ported to src/; help/system UI entry points compile under SDL. |
| `HOUSE.CPP` | `src/house.cpp` | differs | Ported to src/; house logic restored. |
| `IDATA.CPP` | `src/idata.cpp` | differs | Ported to src/; infantry type tables/constructors restored (depends on icon-set map helpers). |
| `INFANTRY.CPP` | `src/infantry.cpp` | differs | Ported to src/; infantry behavior restored. |
| `INI.CPP` | `src/ini.cpp` | differs | Ported to src/; INI parsing/writing now uses portable file helpers. |
| `INIT.CPP` | `src/port_runtime.cpp`, `src/init_helpers.cpp` | manual | Ported via the SDL startup path: `Parse_Command_Line`, `Init_Game`, and `Select_Game` now live in the runtime/initialization helpers. |
| `INTERNET.CPP` |  | manual | To be ported. |
| `INTERPAL.CPP` | `src/interpal.cpp` | identical | Ported to src/; interpolation palette helpers restored. |
| `INTRO.CPP` | `src/intro.cpp` | differs | Ported `Choose_Side()` to the SDL build: restored the CHOOSE.WSA side-pick animation flow and briefing movie playback (`GDI1`/`NOD1PRE`); VQA audio/subtitle parity is still pending. |
| `CONQUER.CPP` (VQA playback path) | `src/movie.cpp`, `src/vqa_decoder.cpp`, `src/vqa_decoder.h` | manual | Replaced the simulated/timing-only movie stub with a real classic (8-bit) Westwood VQA decoder (LCW/Format80) and wired `Play_Movie()` to decode frames, apply per-frame palettes, and blit into the SDL software pages. |
| `IOMAP.CPP` | `src/iomap.cpp` | differs | Ported to src/ with pointer coding helpers for map/UI classes restored. |
| `IOOBJ.CPP` | `src/ioobj.cpp` | differs | Ported to src/ with object save/load pointer coding and smudge/overlay helpers restored. |
| `IPX.CPP` | `src/ipx.cpp` | differs | Ported to src/; IPX manager/connection glue now builds in the SDL target. |
| `IPX95.CPP` | `src/ipx95.cpp` | differs | Implemented the Win95 IPX95 entry points (`IPX_Send_Packet95`/`IPX_Broadcast_Packet95`/`IPX_Get_Outstanding_Buffer95`) on top of the UDP-backed `IPXConnClass`, restoring packet send/receive behavior instead of returning stub values. |
| `IPXADDR.CPP` | `src/ipxaddr.cpp` | differs | Ported to src/; IPX address helpers preserved. |
| `IPXCONN.CPP` | `src/ipxconn.cpp` | differs | Ported to src/; UDP-backed IPXConnClass implementation. |
| `IPXGCONN.CPP` | `src/ipxgconn.cpp` | differs | Ported to src/; connection manager glue preserved. |
| `IPXMGR.CPP` | `src/ipxmgr.cpp` | differs | IPX manager now tracks send/receive counters and computes basic response-time stats from observed traffic, replacing placeholder return values while keeping the UDP transport layer. |
| `JSHELL.CPP` | `src/jshell.cpp` | differs | Ported to src/; JShell UI helpers and icon sampling logic restored. |
| `KEYFRAME.CPP` | `src/keyframe_info.cpp`, `src/keyframe_helpers.cpp` | manual | Frame metadata/palette accessors plus `Build_Frame` decode are ported (Format80/Format40); implemented Win95-style uncompressed frame caching (big/theater buffers + `Reset_Theater_Shapes`) and made `Build_Frame` return a real pointer (`void*`) for 64-bit safety. |
| `LOADDLG.CPP` | `src/loaddlg.cpp` | differs | Ported load/save/delete dialog backend, replacing the placeholder `LoadOptionsClass` implementation and wiring it to the real save/load entry points. |
| `LOAD_TITLE.CPP` | `src/load_title.cpp` | legacy missing | Title screen loader now resolves art through the CCFile/Mix stack, decodes PCX or CPS deterministically, patches UI palette colors, and scales to the active viewport. Fixed PCX RLE decoding so runs that cross scanline boundaries are handled correctly (prevents main menu background corruption). |
| `LOAD_TITLE.CPP` | `src/load_title.cpp` | legacy missing | Title palette normalization now mirrors the legacy PCX loader and no longer overwrites the first 16 palette entries, matching Win95 background colors. |
| `LOGIC.CPP` | `src/logic.cpp` | differs | Ported to src/; game logic loop and orchestration now builds under SDL. |
| `MAIN.CPP` | `src/main.cpp` | legacy missing | SDL bootstrap now requests a high-DPI window and nearest-neighbor scaling so UI/text pixels stay crisp. |
| `WWLIB_RUNTIME.CPP` (present blit) | `src/wwlib_runtime.cpp` | legacy missing | Present texture now pins SDL texture scale mode to `nearest` to avoid blurry text when SDL scales the 8-bit buffer, and ModeX_Blit forwards legacy menu blits to the SDL presenter. |
| `WWLIB_RUNTIME.CPP` (`WWMouseClass`) | `src/wwlib_runtime.cpp` | legacy missing | Implemented software cursor draw/erase (save-under + restore) and wired `Set_Mouse_Cursor` so legacy Hide/Show/Conditional mouse logic works. |
| `MOUSE.CPP` | `src/mouse.cpp` | differs | Ported MouseClass cursor controller (loads `MOUSE.SHP`, supports small variants, and animates cursor frames via the legacy countdown timer). |
| `GSCREEN.CPP` | `src/gscreen.cpp` | differs | Title screen blit texture also forces `nearest` scale mode to keep fonts/pixels sharp. |
| `GSCREEN.CPP` | `src/gscreen.cpp` | differs | Blit_Display now routes through SeenBuff blits with mouse draw/erase hooks so SDL presentation matches the legacy flow and palette conversion stays consistent. |
| `MAP.CPP` | `src/map.cpp` | differs | Ported to src/; MapClass core and binary read/write helpers now build under SDL (parity work still pending). |
| `MAPEDDLG.CPP` |  | manual | To be ported. |
| `MAPEDIT.CPP` | `src/mapedit.cpp` | differs | Legacy map editor module is present in src/ but remains gated behind `SCENARIO_EDITOR` and is not integrated into the SDL target flow yet. |
| `MAPEDPLC.CPP` |  | manual | To be ported. |
| `MAPEDSEL.CPP` | `src/mapedsel.cpp` | identical | Legacy map editor selection/manipulation module is present in src/ but remains gated behind `SCENARIO_EDITOR` and is not integrated into the SDL target flow yet. |
| `MAPEDTM.CPP` |  | manual | To be ported. |
| `MAPSEL.CPP` | `src/mapsel.cpp` | differs | Ported to src/; map selection screen logic preserved (integration/parity verification pending). |
| `MENUS.CPP` | `src/menus.cpp` | differs | Restored the original main menu: renders the title/dialog chrome, builds the legacy button list (including expansions/bonus), pumps SDL events into the keyboard queue, honors timeouts, and returns the canonical selection indices. |
| `MIXFILE.CPP` | `src/mixfile.cpp` | differs | Mix archive reader parses headers, caches payloads, and resolves entries by CRC for asset lookup. |
| `MIXFILE.CPP` | `src/mixfile.cpp` | differs | Added XCC name-table support so mixed archives (e.g., CD1/CCLOCAL.MIX) with embedded filenames resolve fonts correctly. |
| `MONOC.CPP` | `src/monoc.cpp` | differs | Ported monochrome debug buffer to a heap-backed screen page and removed DOS/segment calls while keeping the original text/box routines. |
| `MONOC.H` | `src/include/legacy/monoc.h` | differs | Box character table now uses 8-bit storage to preserve IBM line-draw values without C++ narrowing errors. |
| `MISSION.CPP` | `src/mission.cpp` | differs | Ported to src/ with legacy mission state machine logic intact and includes updated for the SDL build; pointer coding now handled in `src/ioobj.cpp`. |
| `MOUSE.CPP` | `src/mouse.cpp` | differs | Ported MouseClass cursor controller (loads `MOUSE.SHP`, supports small variants, and animates cursor frames via the legacy countdown timer). |
| `MPLAYER.CPP` |  | manual | To be ported. |
| `MSGBOX.CPP` | `src/msgbox.cpp` | differs | Ported to src/; CCMessageBox UI logic restored. |
| `MSGLIST.CPP` | `src/msglist.cpp` | differs | Ported to src/; MessageListClass restored. |
| `NETDLG.CPP` |  | manual | To be ported. |
| `NOSEQCON.CPP` | `src/noseqcon.cpp` | differs | Ported non-sequenced connection queue logic into src/ with modern headers. |
| `NULLCONN.CPP` | `src/nullconn.cpp` | differs | Ported NULL modem connection framing/CRC with UDP-backed send path. |
| `NULLDLG.CPP` |  | manual | To be ported. |
| `NULLMGR.CPP` | `src/nullmgr.cpp` | differs | Ported NULL modem manager with UDP-based transport, queue/timing, and buffer parsing. |
| `OBJECT.CPP` | `src/object.cpp` | differs | Ported to src/; core object hierarchy logic restored. |
| `ODATA.CPP` | `src/odata.cpp` | identical | Ported to src/; overlay type tables/graphics restored. |
| `OPTIONS.CPP` | `src/options.cpp` | differs | Ported to src/; options settings and palette hooks restored, and the score-volume setter no longer recurses into itself. |
| `OVERLAY.CPP` | `src/overlay.cpp` | differs | Ported to src/; overlay object logic restored and map-shim access updated for stubbed cells. |
| `PACKET.CPP` | `src/packet.cpp` | differs | Ported to src/; packet/field linked-list serialization now builds on `FieldClass` with correct host/network byte swapping and dword padding. |
| `POWER.CPP` | `src/power.cpp` | differs | Power bar UI ported; shapes are loaded via the modern MIX helpers and redraw logic mirrors the original radar/sidebar flow. |
| `PROFILE.CPP` | `src/profile.cpp` | differs | Ported Win95 INI/profile helpers (`WWGet/WritePrivateProfile*`, hex/int/string parsing, section/entry deletion) and removed the stub implementations from `src/port_runtime.cpp`; hardened `WWGetPrivateProfileString` to accept LF-only line endings safely (avoids UB pointer comparisons). |
| `QUEUE.CPP` | `src/queue.cpp` | differs | Ported to src/ with mission queue helper logic and pointer coding helpers. |
| `RADAR.CPP` | `src/radar.cpp` | differs | Ported to src/; radar UI helpers now build under SDL. |
| `RAWFILE.CPP` | `src/rawfile.cpp` | differs | RawFileClass rebuilt atop POSIX read/write/seek; `Error()` now mirrors Win95 retry/exit prompts (with a CCMessageBox fallback when fonts are available). |
| `BUFFER_TO_PAGE` (legacy blit) | `src/buffer_to_page.cpp` | legacy missing | Raw 8-bit buffer copy now performs bounds-aware page blits instead of the stub. |
| `REINF.CPP` | `src/reinf.cpp` | differs | Ported to src/ with reinforcement creation logic wired for triggers. |
| `SAVELOAD.CPP` | `src/saveload.cpp` | differs | Ported save/load and misc-value serialization routines (plus pointer coding) so savegame operations have a real implementation again. |
| `SCENARIO.CPP` | `src/scenario.cpp` | differs | Ported to src/; scenario INI loading and theater setup preserved. |
| `SCORE.CPP` | `src/score.cpp` | manual | Ported to src/; score/ending UI helpers and globals restored (verified against legacy aside from safe C++ changes). |
| `SDATA.CPP` | `src/sdata.cpp` | differs | Ported to src/; smudge type tables/graphics restored. |
| `SEQCONN.CPP` | `src/seqconn.cpp` | differs | Ported to src/; sequenced packet connection class preserved for future networking parity work. |
| `SIDEBAR.CPP` | `src/sidebar.cpp` | differs | Ported to src/; sidebar UI logic restored. |
| `SMUDGE.CPP` | `src/smudge.cpp` | differs | Ported to src/; smudge object logic restored and map-shim access updated for stubbed cells. |
| `SOUNDDLG.CPP` | `src/sounddlg.cpp` | differs | Ported to src/; sound controls dialog backend implemented. |
| `SPECIAL.CPP` | `src/special.cpp` | differs | Special options dialog wired with original checkbox logic and OK/Cancel flow. |
| `STARTUP.CPP` | `src/main.cpp`, `src/game.cpp`, `src/port_runtime.cpp` | manual | Ported via the SDL entry point; WinMain-only startup scaffolding is intentionally not compiled into the SDL target. |
| `STATS.CPP` | `src/stats.cpp` | differs | Ported to src/; statistics screens now build. |
| `SUPER.CPP` | `src/super.cpp` | identical | Ported to src/; superweapon logic restored. |
| `TARCOM.CPP` | `src/tarcom.cpp` | differs | Ported to src/; targeting/command logic restored. |
| `TARGET.CPP` | `src/target.cpp` | differs | Ported to src/ with target decoding helpers for units/buildings/cells. |
| `TCPIP.CPP` | `src/tcpip.cpp` | differs | Portable UDP-backed implementation for session messaging (PlanetWestwood globals + basic send/receive path). |
| `TDATA.CPP` | `src/tdata.cpp` | identical | Ported to src/; terrain template/type tables restored. |
| `TEAM.CPP` | `src/team.cpp` | differs | Ported to src/ with legacy team coordination logic and mission routing. |
| `TEAMTYPE.CPP` | `src/teamtype.cpp` | differs | Ported to src/ with team type tables, INI parsing, and mission name helpers intact. |
| `TECHNO.CPP` | `src/techno.cpp` | differs | Ported to src/; shared TechnoClass logic restored. |
| `TEMP.CPP` | `src/temp.cpp` | modern missing | Ported to src/; temporary data helpers preserved. |
| `TEMPLATE.CPP` | `src/template.cpp` | differs | Ported to src/; template object logic restored (depends on icon-set map helpers) and map-shim access updated. |
| `TERRAIN.CPP` | `src/terrain.cpp` | differs | Ported to src/; terrain object logic restored. |
| `THEME.CPP` | `src/theme.cpp` | differs | Theme playback no longer forces mixer-level looping; scores now end naturally so `ThemeClass::AI()` can advance tracks per Win95 shuffle/repeat rules. |
| `TRIGGER.CPP` | `src/trigger.cpp` | differs | Ported to src/ with trigger parsing and execution logic preserved. |
| `TURRET.CPP` | `src/turret.cpp` | differs | Ported to src/; turret control logic restored. |
| `UDATA.CPP` | `src/udata.cpp` | differs | Ported to src/; unit type tables/constructors restored. |
| `UNIT.CPP` | `src/unit.cpp` | differs | Ported to src/; unit behavior restored. |
| `UTRACKER.CPP` | `src/utracker.cpp` | differs | Unit tracker ported with network/PC byte-order conversion helpers. |
| `VISUDLG.CPP` | `src/visudlg.cpp` | differs | Ported to src/; visual controls dialog backend implemented. |
| `WINSTUB.CPP` | `src/port_runtime.cpp` | differs | No longer needed in the SDL target; the globals (`ReadyToQuit`, etc) are defined in the runtime implementation. |
| `ABSTRACT.H` | `src/include/legacy/abstract.h` | differs | Lowercase mirror retained so `#include "abstract.h"` works on case-sensitive hosts. |
| `COMPAT.H` | `src/include/legacy/compat.h` | differs | Palette/buffer macros and legacy globals wrapped in portable defaults. |
| `DEFINES.H` | `src/include/legacy/defines.h` | differs | Lowercase mirror preserving gameplay feature toggles until modernization. |
| `EXTERNS.H` | `src/include/legacy/externs.h` | differs | Lowercase copy to keep the sprawling extern declarations accessible. |
| `FUNCTION.H` | `src/include/legacy/function.h` | differs | Lowercase mirror ensuring the UI hierarchy declarations keep compiling; added icon-set helper declarations used by the template pipeline. |
| `MISSION.H` | `src/include/legacy/mission.h` | differs | Lowercase copy of the mission AI declarations for case-sensitive builds. |
| `OBJECT.H` | `src/include/legacy/object.h` | differs | Lowercase mirror safeguarding the core object hierarchy headers. |
| `REAL.H` | `src/include/legacy/real.h` | differs | Lowercase copy retaining the original fixed-point math helpers. |
| `TARGET.H` | `src/include/legacy/target.h` | differs | Lowercase mirror for the targeting helper declarations. |
| `TYPE.H` | `src/include/legacy/type.h` | differs | Lowercase copy of the shared enums/typedefs used throughout the game. |
| `WWFILE.H` | `src/include/legacy/wwfile.h` | differs | FileClass interface refreshed with `std::size_t` and a namespace alias. |
| `WWLIB32.H` | `src/include/legacy/wwlib32.h` | legacy missing | Navigation key constants now mirror SDL keycodes for menu/input handling; added viewport stamp/scale helpers used by template previews. |
| `WATCOM.H` | `src/include/legacy/watcom.h` | differs | Watcom pragma wrappers swapped for GCC diagnostic helpers. |
| `PLATFORM (new)` | `src/include/legacy/platform.h` | legacy missing | Win16/Watcom typedef shim that turns `near`/`far` keywords into no-ops. |
| `WINDOWS_COMPAT (new)` | `src/include/legacy/windows_compat.h` | legacy missing | Win32 handle/struct typedef shim so the port never includes platform headers directly. |
| `CMakeLists.txt` | `CMakeLists.txt` | identical | Added missing ported sources and gated `src/platform_win32.cpp` behind `WIN32`, then pruned duplicate stub units (`src/base_stub.cpp`, `src/gameplay_minimal_stubs.cpp`, `src/gameplay_shims.cpp`, `src/tiny_linker_shims.cpp`) to avoid duplicate symbols; `src/debug.cpp`/`src/ending.cpp` are now linked. |
| `STARTUP.CPP` (error exit) | `src/error.cpp` + `src/include/legacy/error.h` | differs | Ported `Print_Error_End_Exit`/`Print_Error_Exit` and restored the allocation failure callbacks without dummy `printf/exit` stubs. |
| `MMX.ASM` | `src/mmx.cpp` + `src/include/legacy/mmx.h` | differs | Replaced the MMX detection stub with a CPUID-based probe; the patch-table init is a no-op in the SDL renderer path. |
| `AIRCRAFT.H` | `src/include/legacy/aircraft.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `ANIM.H` | `src/include/legacy/anim.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `AUDIO.H` | `src/include/legacy/audio.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `BASE.H` | `src/include/legacy/base.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `BUILDING.H` | `src/include/legacy/building.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `BULLET.H` | `src/include/legacy/bullet.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `CARGO.H` | `src/include/legacy/cargo.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `CCDDE.H` | `src/include/legacy/ccdde.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `CCFILE.H` | `src/include/legacy/ccfile.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `CDFILE.H` | `src/include/legacy/cdfile.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `CELL.H` | `src/include/legacy/cell.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `CHECKBOX.H` | `src/include/legacy/checkbox.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `CHEKLIST.H` | `src/include/legacy/cheklist.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `COLRLIST.H` | `src/include/legacy/colrlist.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `COMBUF.H` | `src/include/legacy/combuf.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `COMQUEUE.H` | `src/include/legacy/comqueue.h` | differs | Reuses the combuf queue entry structs to avoid duplicate typedef conflicts. |
| `CONFDLG.H` | `src/include/legacy/confdlg.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `CONNECT.H` | `src/include/legacy/connect.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `CONNMGR.H` | `src/include/legacy/connmgr.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `CONQUER.H` | `src/include/legacy/conquer.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `CONTROL.H` | `src/include/legacy/control.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `CREDITS.H` | `src/include/legacy/credits.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `CREW.H` | `src/include/legacy/crew.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `DDE.H` | `src/include/legacy/dde.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `DEBUG.H` | `src/include/legacy/debug.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `DESCDLG.H` | `src/include/legacy/descdlg.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `DIAL8.H` | `src/include/legacy/dial8.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `DISPLAY.H` | `src/include/legacy/display.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `DOOR.H` | `src/include/legacy/door.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `DPMI.H` | `src/include/legacy/dpmi.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `DRIVE.H` | `src/include/legacy/drive.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `EDIT.H` | `src/include/legacy/edit.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `ENDING.H` | `src/include/legacy/ending.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `EVENT.H` | `src/include/legacy/event.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `FACING.H` | `src/include/legacy/facing.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `FACTORY.H` | `src/include/legacy/factory.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `FIELD.H` | `src/include/legacy/field.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `FLASHER.H` | `src/include/legacy/flasher.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `FLY.H` | `src/include/legacy/fly.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `FOOT.H` | `src/include/legacy/foot.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `FUSE.H` | `src/include/legacy/fuse.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `GADGET.H` | `src/include/legacy/gadget.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `GAMEDLG.H` | `src/include/legacy/gamedlg.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `GAUGE.H` | `src/include/legacy/gauge.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `GOPTIONS.H` | `src/include/legacy/goptions.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `GSCREEN.H` | `src/include/legacy/gscreen.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `HEAP.H` | `src/include/legacy/heap.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `HELP.H` | `src/include/legacy/help.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `HOUSE.H` | `src/include/legacy/house.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `INFANTRY.H` | `src/include/legacy/infantry.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `INTRO.H` | `src/include/legacy/intro.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `IPX.H` | `src/include/legacy/ipx.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `IPX95.H` | `src/include/legacy/ipx95.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `IPXADDR.H` | `src/include/legacy/ipxaddr.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `IPXCONN.H` | `src/include/legacy/ipxconn.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `IPXGCONN.H` | `src/include/legacy/ipxgconn.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `IPXMGR.H` | `src/include/legacy/ipxmgr.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `JSHELL.H` | `src/include/legacy/jshell.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `LAYER.H` | `src/include/legacy/layer.h` | differs | Layer container header now uses `#pragma once`, clean overrides, and forward decls. |
| `LED.H` | `src/include/legacy/led.h` | differs | Keyboard LED helper moved to `#pragma once` and `std::uint8_t`-based control flags. |
| `LINK.H` | `src/include/legacy/link.h` | differs | Doubly linked helper now uses `#pragma once` plus `[[nodiscard]]` accessors for list traversal. |
| `LIST.H` | `src/include/legacy/list.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `LOADDLG.H` | `src/include/legacy/loaddlg.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `LOGIC.H` | `src/include/legacy/logic.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `MAP.H` | `src/include/legacy/map.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `MAPEDIT.H` | `src/include/legacy/mapedit.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `MEMCHECK.H` | `src/include/legacy/memcheck.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `MESSAGE.H` | `src/include/legacy/message.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `MIXFILE.H` | `src/include/legacy/mixfile.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `MONOC.H` | `src/include/legacy/monoc.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `MOUSE.H` | `src/include/legacy/mouse.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `MSGBOX.H` | `src/include/legacy/msgbox.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `MSGLIST.H` | `src/include/legacy/msglist.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `NOSEQCON.H` | `src/include/legacy/noseqcon.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `NULLCONN.H` | `src/include/legacy/nullconn.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `NULLMGR.H` | `src/include/legacy/nullmgr.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `OPTIONS.H` | `src/include/legacy/options.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `OVERLAY.H` | `src/include/legacy/overlay.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `PACKET.H` | `src/include/legacy/packet.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `PHONE.H` | `src/include/legacy/phone.h` | differs | Phone book entries now rely on `<cstring>` comparisons and zeroed buffers. |
| `POWER.H` | `src/include/legacy/power.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `QUEUE.H` | `src/include/legacy/queue.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `RADAR.H` | `src/include/legacy/radar.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `RADIO.H` | `src/include/legacy/radio.h` | differs | Radio contact logic now uses `nullptr`, defaulted special members, and consistent pointer helpers. |
| `RAWFILE.H` | `src/include/legacy/rawfile.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `REGION.H` | `src/include/legacy/region.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `SAVEDLG.H` | `src/include/legacy/savedlg.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `SCORE.H` | `src/include/legacy/score.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `SCREEN.H` | `src/include/legacy/screen.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `SCROLL.H` | `src/include/legacy/scroll.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `SEQCONN.H` | `src/include/legacy/seqconn.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `SESSION.H` | `src/include/legacy/session.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `SHAPEBTN.H` | `src/include/legacy/shapebtn.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `SIDEBAR.H` | `src/include/legacy/sidebar.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `SLIDER.H` | `src/include/legacy/slider.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `SMUDGE.H` | `src/include/legacy/smudge.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `SOUNDDLG.H` | `src/include/legacy/sounddlg.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `SPECIAL.H` | `src/include/legacy/special.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `STAGE.H` | `src/include/legacy/stage.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `SUPER.H` | `src/include/legacy/super.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `TAB.H` | `src/include/legacy/tab.h` | differs | Sidebar tab header now uses `#pragma once`, overrides, and inline helpers for redraws. |
| `TARCOM.H` | `src/include/legacy/tarcom.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `TCPIP.H` | `src/include/legacy/tcpip.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `TEAM.H` | `src/include/legacy/team.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `TEAMTYPE.H` | `src/include/legacy/teamtype.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `TECHNO.H` | `src/include/legacy/techno.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `TEMPLATE.H` | `src/include/legacy/template.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `TERRAIN.H` | `src/include/legacy/terrain.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `TEXTBLIT.H` | `src/include/legacy/textblit.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `TEXTBTN.H` | `src/include/legacy/textbtn.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `THEME.H` | `src/include/legacy/theme.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `TOGGLE.H` | `src/include/legacy/toggle.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `TRIGGER.H` | `src/include/legacy/trigger.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `TURRET.H` | `src/include/legacy/turret.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `TXTLABEL.H` | `src/include/legacy/txtlabel.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `UNIT.H` | `src/include/legacy/unit.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `UTRACKER.H` | `src/include/legacy/utracker.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `VISUDLG.H` | `src/include/legacy/visudlg.h` | identical | Lowercase mirror retained for Linux-friendly includes. |
| `WWALLOC.H` | `src/include/legacy/wwalloc.h` | differs | Lowercase mirror retained for Linux-friendly includes. |
| `MAP_SHIM.H` | `src/include/legacy/map_shim.h` | legacy missing | Map compatibility header updated with missing shim helpers/fields (cell effects + theater) needed by drive/overlay/terrain logic during the SDL bring-up. |
| `MAP_SHIM.H` | `src/include/legacy/map_shim.h` | legacy missing | Added missing sidebar/radar shim members plus gadget helpers for the sidebar build path. |
| `MAP_SHIM.CPP` | `src/map_shim.cpp` | legacy missing | Initialized radar geometry and wired shim add/remove button helpers into the shared button list. |
| `WWLIB32.H` | `src/include/legacy/wwlib32.h` | legacy missing | Added `KN_TAB` and `Set_Font` helper to align sidebar/font usage with SDL key constants. |
| `OPTIONS.H` | `src/include/legacy/options.h` | differs | Added legacy `Set_Score_Vol` alias for score volume control. |
| `FUNCTION.H` | `src/include/legacy/function.h` | differs | Declared palette helpers and RGB/HSV conversion utilities used by options and score flows. |
| `DISPLAY.CPP` | `src/display.cpp` | differs | Added palette setter plus HSV conversion helpers to support options palette adjustments. |
| `MSGLIST.CPP` | `src/msglist.cpp` | differs | Included the message list header so the class definitions are complete for compilation. |
| `MSGLIST.H` | `src/include/legacy/msglist.h` | differs | Added required includes so message list types/constants resolve. |
| `TXTLABEL.H` | `src/include/legacy/txtlabel.h` | differs | Added missing includes for gadget and text flag dependencies. |
| `WWLIB32.H` | `src/include/legacy/wwlib32.h` | legacy missing | Added legacy key constants and new blit/to-buffer helper declarations. |
| `WWLIB_RUNTIME.CPP` | `src/wwlib_runtime.cpp` | legacy missing | Implemented buffer blit overload and viewport copy helpers for message dialogs. |
| `FUNCTION.H` | `src/include/legacy/function.h` | differs | Declared Buffer_To_Page overloads for viewports. |
| `BUFFER_TO_PAGE.CPP` | `src/buffer_to_page.cpp` | legacy missing | Added viewport overloads for raw buffer copies. |
| `FTIMER.H` | `src/include/legacy/ftimer.h` | differs | Added CountDownTimerClass start helper for modal message timing. |
| `WWLIB32.H` | `src/include/legacy/wwlib32.h` | legacy missing | Added viewport blit overloads for message box restore paths. |
| `WWLIB_RUNTIME.CPP` | `src/wwlib_runtime.cpp` | legacy missing | Implemented viewport blit overload for buffer restore in dialogs. |
| `WWLIB32.H` | `src/include/legacy/wwlib32.h` | legacy missing | Added buffer-to-buffer blit overloads used by message box saves. |
| `WWLIB_RUNTIME.CPP` | `src/wwlib_runtime.cpp` | legacy missing | Implemented buffer-to-buffer blit for dialog buffer preservation. |
| `WWLIB32.H` | `src/include/legacy/wwlib32.h` | legacy missing | Added logic-page overloads and text print helpers for score rendering. |
| `WWLIB_RUNTIME.CPP` | `src/wwlib_runtime.cpp` | legacy missing | Implemented buffer-backed logic pages and buffer text printing for score flows. |
| `FUNCTION.H` | `src/include/legacy/function.h` | differs | Declared Play_Sample for score audio hooks. |
| `AUDIO.CPP` | `src/audio.cpp` | differs | Removed duplicate default argument on Play_Sample declaration. |
| `FUNCTION.H` | `src/include/legacy/function.h` | differs | Added WSA animation declarations and StreamLowImpact flag used by score screens. |
| `PORT_STUBS.CPP` | `src/port_runtime.cpp` | legacy missing | Ported WSA/SHP drawing entry points: `Open_Animation`/`Animate_Frame`/`Get_Animation_Frame_Count`/`Close_Animation`, `Extract_Shape_Count`, and `CC_Draw_Shape` now decode and blit frames (incl. fade/translucency tables) for score/UI flows. |
| `WWLIB32.H` | `src/include/legacy/wwlib32.h` | legacy missing | Added buffer blit/fill helpers needed by score screen drawing. |
| `WWLIB_RUNTIME.CPP` | `src/wwlib_runtime.cpp` | legacy missing | Implemented buffer blit/fill helpers and Check_Key shim for score loops. |
| `FUNCTION.H` | `src/include/legacy/function.h` | differs | Added Close_Animation/Check_Key/Extract_Shape_Count declarations for score flow. |
| `PORT_STUBS.CPP` | `src/port_runtime.cpp` | legacy missing | Removed placeholder Close_Animation/Extract_Shape_Count by wiring them into the real keyframe decoder. |
| `SCORE.CPP` | `src/score.cpp` | manual | Fixed palette type and loop variable scoping for modern C++ builds. |
| `WWLIB32.H` | `src/include/legacy/wwlib32.h` | legacy missing | Added KN_Q/KA_TILDA constants and buffer line drawing hook. |
| `WWLIB_RUNTIME.CPP` | `src/wwlib_runtime.cpp` | legacy missing | Implemented buffer line drawing and Get_Key shim. |
| `FUNCTION.H` | `src/include/legacy/function.h` | differs | Added Get_Key declaration for score name entry. |
| `SCORE.CPP` | `src/score.cpp` | manual | Updated multi-score palettes to unsigned char to avoid narrowing errors. |
| `SCORE.CPP` | `src/score.cpp` | manual | Aligned multi-score palette pointer types with unsigned char buffers. |
| `WWLIB32.H` | `src/include/legacy/wwlib32.h` | legacy missing | Added lock/offset and pitch helpers needed by palette interpolation. |
| `WWLIB_RUNTIME.CPP` | `src/wwlib_runtime.cpp` | legacy missing | Implemented buffer/view offsets plus lock helpers for interpolation paths. |
| `FUNCTION.H` | `src/include/legacy/function.h` | differs | Added Wait_Blit declaration for interpolation flow. |
| `PORT_STUBS.CPP` | `src/port_runtime.cpp` | legacy missing | Implemented `Wait_Blit` to pump SDL events and yield (keeps palette interpolation flows responsive and closer to the Win95 “wait for blitter/vblank” intent). |
| `DISPLAY.CPP` | `src/display.cpp` | differs | Updated `Fade_Palette_To` so fades are visible even when callers pass `NULL` and paced in Win95-style 60Hz ticks. |
| `WWLIB_RUNTIME.CPP` | `src/wwlib_runtime.cpp` | legacy missing | Adjusted palette expansion to Win95-style 6-bit→8-bit (`<<2`) and added a palette LUT to keep SDL presentation behavior deterministic and fast. |
| `SMUDGE.H` | `src/include/legacy/smudge.h` | differs | Included `wwfile.h` so FileClass I/O methods resolve. |
| `SDATA.CPP` | `src/sdata.cpp` | differs | Added missing legacy includes for theater/mix/map helpers used by smudge setup. |
| `HELP.CPP` | `src/help.cpp` | differs | Added HelpClass destructor definition to satisfy vtable linkage. |
| `CELL.CPP` | `src/cell.cpp` | differs | Ported Spot_Index/Clear_Icon helpers used by infantry and radar plots. |
| `GLOBALS.CPP` | `src/globals.cpp` | differs | Restored ModemGameToPlay global so network flows link. |
| `INTERPAL` | `src/interpal_asm.cpp` | legacy missing | Ported the legacy palette interpolation and 2x scaling routines (previously ASM) as portable C++ while preserving Win95 behavior. |
| `KEYFRAME.CPP` | `src/keyframe_helpers.cpp` | differs | Replaced the remaining uncompressed-shape “stub” behavior by caching decoded frames into Win95-style big/theater shape buffers and wiring `Reset_Theater_Shapes`; cache hits copy pixels into the caller buffer to preserve existing SDL draw paths. |
| `TEXT.CPP` | `src/text.cpp` | legacy missing | Added Simple_Text_Print wrapper for legacy text calls. |
| `INIT.CPP` | `src/init_helpers.cpp` | differs | Ported Obfuscate helper for hidden option parsing. |
| `SCENARIO.CPP` | `src/scenario.cpp` | differs | Ported scenario load/start + win/lose/restart flow so the menu can launch missions. |
| `INI.CPP` | `src/ini.cpp` | differs | Ported scenario INI naming + parsing so `Start_Scenario()` can load mission data. |
| `SAVELOAD.CPP` | `src/saveload_helpers.cpp` | differs | Ported TechnoType target conversions for save/load pointer coding. |
| `CONQUER.CPP` | `src/conquer_helpers.cpp` | differs | Added selection helpers, radar icon builder, and keyboard handler for tactical input. |
| `NETDLG/NULLDLG` | `src/network_helpers.cpp` | legacy missing | Added network disconnect/reconnect dialog helpers for queue handling. |
| `WINSTUB` | `src/pcx_write.cpp` | legacy missing | Implemented PCX writer for screenshot/debug output. |
| `GAMEDLG.CPP` | `src/gamedlg.cpp` | differs | Ported game controls dialog processing. |
| `LOADDLG.CPP` | `src/loaddlg.cpp` | differs | Re-synced the Win95 load/save/delete dialog: adds the SAVE “Empty Slot” entry, preserves “(old)” save markers, sorts newest-first, enforces description + disk-space rules, prompts on delete, and matches the original exit/refresh flow. |
| `MAP.CPP` / `LOGIC.CPP` | `src/map.cpp` / `src/logic.cpp` | differs | Restored the original map/logic runtime and removed the linked gameplay/map stub units; fixed `MapClass::Read_Binary` to read packed `.BIN` map cell streams (no struct padding), eliminating the scenario-load crash when starting new campaigns. |
| `PORT_STUBS.CPP` | `src/port_runtime.cpp` | legacy missing | Added global animation helper definitions for link parity. |
| `CRC` | `src/crc_helpers.cpp` | legacy missing | Added Calculate_CRC helper used by obfuscation and legacy CRC checks. |
| `FUNCTION.H` | `src/include/legacy/function.h` | differs | Declared Calculate_CRC helper for obfuscation and network CRC usage. |
| `FUNCTION.H` | `src/include/legacy/function.h` | differs | Declared Get_Shape_Header_Data helper for radar icon extraction. |
| `BITBLT` | `src/bitblt_helpers.cpp` | legacy missing | Implemented the original `Bit_It_In_Scale` pixel-fade blit effect (randomized copy with optional “dagger” overlay) instead of a simple blit placeholder. |
| `SOUNDDLG.CPP` / `VISUDLG.CPP` | `src/sounddlg.cpp`, `src/visudlg.cpp` | manual | Ported the sound/visual controls dialogs so the options flow uses real handlers (no placeholder behavior). |
| `MOUSE.H` | `src/mouse_vtable.cpp` | differs | Defined MouseClass VTable storage to satisfy serialization references. |
| `LOADDLG.H` | `src/include/legacy/loaddlg.h` | differs | Added missing includes for vector/list/defines types. |
| `HEAP.CPP` / `COMBUF.CPP` | `src/heap.cpp`, `src/combuf.cpp` | manual | Restored original heap Save/Load pointer coding flow and ported `CommBufferClass::Mono_Debug_Print2` so networking debug output matches the Win95 mono view again. |
| `FLY_STUB.CPP` |  | manual | `src/fly_stub.cpp` retired after porting `src/fly.cpp`; prior cleanup removed duplicate As_Movement_Coord stub to resolve linker conflicts. |
| `BUILD FIXES` | `CMakeLists.txt` | legacy missing | Made `TD_ENABLE_WERROR=ON` build reliably on Clang/AppleClang by suppressing legacy-warning classes while the port is in progress. |
| `BUILD FIXES` | `src/*.cpp` | legacy missing | Cleaned up a handful of warnings that commonly break strict builds (snprintf, signed/unsigned comparisons, missing default cases, and NULL-to-integer conversions). |
| `CCDDE.CPP` | `src/ccdde.cpp` | differs | Replaced the `Send_Data_To_DDE_Server` stub with a portable UDP localhost implementation for launcher/lobby integration. |
| `DDE.CPP` | `src/dde.cpp` | differs | Implemented a cross-platform DDE replacement using loopback UDP sockets (client poke + optional server bind) to preserve the legacy API surface. |
| `PORT_STUBS.CPP` | `src/port_runtime.cpp` | legacy missing | Removed the duplicate `Send_Data_To_DDE_Server` stub so the CCDDE implementation is authoritative. |
| Tracking | `PROGRESS.md`, `NEXT_STEPS.md`, `README.md` | manual | Removed stale “stubbed” wording where implementations are now present (DDE/CCDDE/TCPIP/Map shim notes) and kept follow-ups scoped to incomplete subsystems. |
| `INTRO.CPP` (`Choose_Side`) | `src/intro.cpp`, `src/port_runtime.cpp`, `src/include/legacy/function.h` | manual | Replaced the temporary message-box stub with the legacy choose-side animation flow and briefing playback, fixed `Load_Alloc_Data(FileClass&)` to allocate via `Alloc()` so `Free()` is safe, and added a portable `Wait_Vert_Blank()` hook used by legacy UI code. |
| `DISPLAY.CPP` | `src/display.cpp` | differs | Restored `DisplayClass::Compute_Start_Pos`/`Write_INI` implementations needed by scenario INI workflows. |
| `FUNCTION.H` | `src/include/legacy/function.h` | differs | Declared `Invalidate_Cached_Icons` and implemented it for the SDL icon path. |
| `MPLAYER.CPP` (`Surrender_Dialog`) | `src/port_runtime.cpp` | differs | Ported the in-game surrender confirmation dialog (OK/Cancel) and removed the unconditional “accept” stub. |
| `WWALLOC` (`Ram_Free`/`Heap_Size`) | `src/alloc.cpp` | legacy missing | Removed placeholder “infinite RAM” return; allocations are now tracked with a size header and `Ram_Free` reports remaining bytes based on `SDL_GetSystemRAM()`. |
| `DEBUG/STARTUP` | `src/port_debug.h`, `src/maingame.cpp` | manual | Added verbose startup tracing gated by `TD_VERBOSE=1` / `--verbose` / `--debug`, plus an opt-in `TD_AUTOSTART_SCENARIO=SCG01EA` debug hook (with `TD_AUTOSTART_LOAD_ONLY=1`) to reproduce scenario loads without driving the main menu. |
| `SCENARIO/INI TRACE` | `src/scenario.cpp`, `src/ini.cpp` | manual | Added verbose tracing around `Clear_Scenario()` and `Read_Scenario_Ini()` (file availability/size/read) to pinpoint hangs during scenario load. |
| `MIXFILE/ASSETS` | `src/mixfile.cpp` | legacy missing | Fixed XCC name-table mapping to use the original on-disk subblock order (sorted CRC table is now kept only for lookups), restoring correct asset retrieval for some CD data sets. |
| `AUDIO/SDL` | `src/include/legacy/audio.h`, `src/platform_audio_sdl.cpp` | manual | Corrected the legacy `Audio_Init` parameter meaning (bits/stereo/rate/buffer) and allow SDL to negotiate spec changes, fixing “Unsupported number of audio channels” on startup. |
| `TEXT/FONTS` | `src/text.cpp`, `src/game.cpp` | manual | Stopped treating gradient palette/table resources as glyph fonts (6pt + green12) and adjusted score font selection to use the real glyph font. |
| `TEXT/GRADIENT` | `src/text.cpp` | legacy missing | Restored gradient font selection (`TPF_6PT_GRAD`/`TPF_GREEN12_GRAD`), fixed high/low-nibble ColorXlat lookup parity, and implemented `TPF_USE_GRAD_PAL` ramp generation (plus medium/bright handling) for the SDL text renderer. |
| `VIDEO/BLIT` | `src/include/legacy/wwlib32.h`, `src/wwlib_runtime.cpp`, `src/gscreen.cpp` | manual | Restored Win95 viewport-to-viewport blit semantics (`source.Blit(dest, ...)` like `HidPage.Blit(SeenBuff)`), ensuring the title/menu pages actually present in the SDL build. |
| `PALETTE/TEXT` | `src/load_title.cpp`, `src/include/legacy/compat.h`, `src/include/legacy/wwlib32.h`, `src/wwlib_runtime.cpp`, `src/text.cpp` | manual | Matched Win95 title/menu palette behavior (apply full PCX palette), corrected base UI color indices (`TBLACK` vs `BLACK`, `GREEN`, etc), and implemented the legacy ColorXlat-driven `Set_Font_Palette` text color path. |
| `CCFILE.CPP` | `src/ccfile.cpp` | differs | Re-synced `CCFileClass` to the original Win95 implementation (MIX open/read/seek semantics + override behavior), restored the CD-availability error gate, and reinstated the legacy `Open_File`/`Read_File`/`Seek_File`/`WWDOS_Shutdown` compatibility entry points. |
| `SIDEBAR.CPP` (clock translucent table) | `src/sidebar.cpp`, `src/conquer_helpers.cpp` | manual | Sidebar clock translucent-table cache now loads from disk when present (working directory or SDL pref path) and regenerates/writes when missing, restoring the original “only rebuild when theater changes” guard and avoiding missing `*CLOCK.MRF` errors. |
| `TIMING` | `src/port_runtime.cpp`, `src/include/legacy/wwlib32.h`, `src/wwlib_runtime.cpp` | manual | Restored canonical time units: `Options.GameSpeed` controls frame cadence again and `TimerClass` reports 60Hz ticks (Win95 semantics); focus loss pauses simulation updates in the SDL main loop. |
| `STARTUP/CONFIG` | `src/game.cpp`, `src/keyframe_helpers.cpp`, `src/port_runtime.cpp`, `src/globals.cpp` | manual | Ported early `CONQUER.INI` setup parsing (`Read_Setup_Options`), restored compressed-shape decision (`Check_Use_Compressed_Shapes`), implemented real disk/RAM probes + memory-error dialog path, and wired multiplayer scenario description cleanup to the real global vectors (no more missing-symbol placeholders). |
| `GOPTIONS.CPP` | `src/goptions.cpp` | differs | Restored Win95 options-dialog persistence by saving settings (`Options.Save_Settings()`) when resuming from the in-game options overlay. |
| `PROFILE PATHS` | `src/port_paths.cpp`, `src/options.cpp`, `src/game.cpp`, `src/port_runtime.cpp`, `src/port_setup.cpp` | manual | Added SDL pref-path backed config resolution so `CONQUER.INI` reads prefer the working directory but fall back to a per-user location; writes go to the same resolved path. |
| `CDFILE/CCFILE` | `src/cdfile.cpp`, `src/ccfile.cpp` | manual | Fixed search-drive path duplication and ensured mirror probing tries all candidates before the fallback open can trigger fatal errors/retry prompts; drive selection now rebuilds the search list so the chosen mirror is preferred, and MIX member lookups ignore directory prefixes so assets resolve regardless of expanded `CD/...` filenames. |
| `MOVIE/VQA` | `src/movie.cpp` | legacy missing | Treat scenario/movie name `x`/`X` as the canonical “no movie” sentinel (Win95 INIs default to `x`), preventing erroneous attempts to open `x.VQA`. |
| `FILE ERROR` | `src/rawfile.cpp`, `src/error.cpp` | manual | File-open failures now fail fast (no retry prompt); fatal-exit no longer pauses for key input unless `TD_PAUSE_ON_FATAL=1` is set. |
| `FINDPATH.CPP` | `src/findpath.cpp` | differs | Fixed edge-follow detour setup (copy direction + command/overlap buffer seeding) to prevent negative `Mem_Copy` sizes and startup crashes in `FootClass::Find_Path()`. |
| `MIXFILE.H` / `MIXFILE.CPP` | `src/include/legacy/mixfile.h`, `src/mixfile.cpp` | manual | Added `MixFileClass::Size_For_Pointer()` to recover a safe byte span for data pointers returned by `Retrieve()` (used to bound decoders). |
| `AUDIO` (AUD decode/mixer) | `src/audio_play.cpp` | legacy missing | Bounded AUD decoding to the containing MIX subfile (or speech buffer) so corrupted/partial inputs can’t walk off-buffer during decode/mix. |
| `AUDIO` (mixer pan/priority) | `src/audio_play.cpp` | manual | Implemented stereo panning (legacy -32767..32767 pan range) and priority-based voice limiting to better match Win95 channel behavior under load. |
| `MAINGAME.CPP` | `src/maingame.cpp` | legacy missing | Extended the `TD_AUTOSTART_SCENARIO` debug hook with `TD_AUTOSTART_LOAD_TITLE`, `TD_AUTOSTART_DRAW_ONCE`, and `TD_AUTOSTART_FRAMES=N` to reproduce scenario-start issues without driving the main menu. |
| `INTERNET.CPP` | `src/internet.cpp` | differs | Ported the Planet Westwood / WChat glue: parses `C&CSPAWN.INI` (from disk or the portable UDP-backed `DDEServer`), restores `Read_Game_Options`, and adds env-driven `Spawn_WChat`/registration hooks for the cross-platform build (`TD_WCHAT_CMD`, `TD_REGISTER_CMD`, `TD_WCHAT_USER`). |
| `BUILD FIXES` (`Map` global / headers) | `src/include/legacy/externs.h`, `src/include/legacy/function.h`, `src/include/legacy/real.h`, `src/cell.cpp`, `src/include/legacy/gscreen.h`, `src/globals.cpp`, `src/unit.cpp` | manual | Restored canonical `Map` type (`MouseClass`) and moved `CellClass::Cell_Number()` out of headers to avoid incomplete-type issues; made `GScreenClass` mouse-shape methods non-pure to match the SDL implementation; fixed Clang `\\x..` escape swallowing in mono debug UI strings. |

## Pending follow-ups
- `COORD PACKING` (`XYP_COORD`): made `XYP_COORD(x,y)` mask before shifting so negative Y offsets don’t invoke undefined behavior under Clang/GCC while preserving the legacy 16-bit high-word packing (`src/include/legacy/function.h`, `src/include/legacy/real.h`).
- `BUILD HYGIENE`: removed an unused local `RedrawType` typedef and annotated legacy layout fields with `[[maybe_unused]]` so `-DTD_ENABLE_WERROR=ON` builds cleanly (`src/loaddlg.cpp`, `src/include/legacy/sounddlg.h`).
- Improve SDL audio mixer parity (channel reservation, fade/stop semantics) and implement streaming/music (ThemeClass).
- Verify the `TPF_USE_GRAD_PAL` gradient ramp selection matches Win95 shading (current implementation derives a 16-step ramp from the active palette by nearest-color search).
