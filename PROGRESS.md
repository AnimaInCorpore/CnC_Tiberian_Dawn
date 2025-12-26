| Legacy file | Modern path | Notes |
| --- | --- | --- |
| `GLOBALS.CPP` | `src/globals.cpp` | Ported to src/, replaced NULL with nullptr, and disabled legacy networking code. |
| `FTIMER.H` | `src/include/ftimer.h` | Countdown timer helper rewritten with `#pragma once` and the global `Frame` counter. |
| `RAND.H` | `src/rand.h` | Random helper declarations cleaned up to use `<cstdint>` types. |
| `RAND.CPP` | `src/rand.cpp` | Random lookup table logic now relies on standard headers and explicit scaling. |
| `VECTOR.H` | `src/vector.h` | Vector template modernized with RAII allocation and portable guards. |
| `VECTOR.CPP` | `src/vector.cpp` | Boolean vector utilities rewritten around clear helpers and `std::memcpy`. |
| `LINK.CPP` | `src/link.cpp` | Doubly linked helper now uses `nullptr` checks and RAII-friendly removal. |
| `LAYER.CPP` | `src/layer.cpp` | Layer manager handles sorting; load/save and pointer coding live in `src/ioobj.cpp`. |
| `LIST.CPP` | `src/list.cpp` | List gadget clamps indices, wires the scroll peers, and manages slider state in pure C++. |
| `RADIO.CPP` | `src/radio.cpp` | Radio messaging logic wrapped in portable helpers with `nullptr`-safe contact handling. |
| `TAB.CPP` | `src/tab.cpp` | Sidebar tab UI now lives in `src/` with `nullptr` guards and cleaned text drawing helpers. |
| `TOGGLE.CPP` | `src/toggle.cpp` | Windows-style toggle gadget refactored with explicit hover handling and sticky state flow. |
| `TXTLABEL.CPP` | `src/txtlabel.cpp` | Text label gadget retained the quirky format print path with optional clipping width tracking. |
| `CHECKBOX.CPP` | `src/checkbox.cpp` | Checkbox gadget draws the interior using the shared box helpers and keeps mouse flicker hidden. |
| `CHEKLIST.CPP` | `src/cheklist.cpp` | Check list delegates selection to `ListClass` then toggles the inline checkmark unless read-only. |
| `COLRLIST.CPP` | `src/colrlist.cpp` | Color-aware list tracks a parallel color vector and draws selected entries using the requested style. |
| `EDIT.CPP` | `src/edit.cpp` | Edit control rebuilt with explicit focus handling, read-only guards, and caret drawing driven by the modern viewport helpers. |
| `GAUGE.CPP` | `src/gauge.cpp` | Gauge widgets now render their fill/tri-color bars via the modern viewport helpers and honor sticky mouse drags. |
| `SHAPEBTN.CPP` | `src/shapebtn.cpp` | Shape buttons wrap the common `ToggleClass` flow and pick the correct frame for disabled/pressed/reflect modes. |
| `SLIDER.CPP` | `src/slider.cpp` | Slider gadget now owns its plus/minus peers, recalculates thumb geometry, and handles clicks outside the thumb area. |
| `SCROLL.CPP` | `src/scroll.cpp` | Map edge scrolling now clamps the virtual cursor, honors inertia, and keeps the mouse override logic intact. |
| `TEXTBTN.CPP` | `src/textbtn.cpp` | Text buttons resize around the assigned label, draw Windows-style boxes, and respect the green gradient flags. |
| `TEXT.CPP` | `src/text.cpp` | Font loader now registers CD MIX archives before loading fonts and applies the legacy X spacing when measuring/drawing strings. |
| `TEXT.CPP` | `src/text.cpp` | Text rendering now requires the real CD font assets (no built-in fallback), validates header blocks, and reads glyph data via absolute offsets to fix garbled menu text. |
| `AADATA.CPP` | `src/aadata.cpp` | Ported to src/, replaced NULL with nullptr, `stricmp` with `strcasecmp`, `sprintf` and `_makepath` with `std::string` manipulation, and updated include path for `function.h`. |
| `ADATA.CPP` | `src/adata.cpp` | Ported to src/, updated include to the legacy shim, and replaced `_makepath` with `std::string` construction for SHP lookup. |
| `ABSTRACT.CPP` | `src/abstract.cpp` | Abstract base logic now reports the correct RTTI/ownable mask, preserves the building distance fudge, and copies INI names with `std::strncpy`. |
| `AIRCRAFT.CPP` | `src/aircraft.cpp` | Full aircraft gameplay/AI logic moved to src/, restoring the original missions, drawing, and targeting behavior; INI serialization now uses bounded snprintf formatting. |
| `TEXT.CPP` (ColorXlat) | `src/text.cpp` | Glyph draw path now mirrors the legacy ColorXlat translation so gradient/LED fonts use their palette ramps instead of a flat foreground fill. |
| `TXTPRNT.ASM` | `src/txtprnt.cpp` | Replaced assembly `Buffer_Print` and `ColorXlat` with portable implementations; provides `Buffer_Print()` and `Get_Font_Palette_Ptr()`. |
| `TEXT.CPP` (spacing) | `src/text.cpp` | Font X/Y spacing now mirrors the legacy Simple_Text_Print rules per font/shadow flag, fixing misaligned SDL text runs. |
| `TEXTBTN.CPP` | `src/textbtn.cpp` | Button labels now center horizontally/vertically using the active font height so menu text aligns cleanly inside the boxes. |
| `DIALOG.CPP` (`Draw_Box`/`CC_Texture_Fill`) | `src/dialog.cpp` | Restored the Win95 `Draw_Box` renderer (including textured green fills via `BTEXTURE.SHP`) and removed the solid-fill stub implementation. |
| `WWLIB32` (video/mouse/surfaces) | `src/wwlib_runtime.cpp`, `src/include/legacy/wwlib32.h` | Replaced the remaining wwlib runtime stubs with SDL-backed video mode updates, real cursor clipping, and focus/restore tracking via `AllSurfaces`. |
| `ALLOC.CPP` | `src/alloc.cpp` | Legacy allocator collapsed onto the modern malloc-based wrapper (moved from the wwalloc port) while keeping the legacy entry points. |
| `ANIM.CPP` | `src/anim.cpp` | Animation system moved to src/; keeps spawn/attach logic, scorch/crater side effects, and translucent draw path intact with nullptr-safe ownership handling; load/save and pointer coding now come from `src/ioobj.cpp`. |
| `AUDIO.CPP` | `src/audio.cpp` | Ported to src/; EVA/sfx logic retained and playback decision path wired. SDL playback backend lives in `src/audio_play.cpp` (AUD decode + mixer + theme helpers). |
| `BASE.CPP` | `src/base.cpp` | Ported to src/, implements base list parsing, save/load and node lookup. |
| `BBDATA.CPP` | `src/bbdata.cpp` | BulletTypeClass definitions and tables ported; shape loading now uses std::string paths and the modern file helpers. |
| `BDATA.CPP` | `src/bdata.cpp` | Building type tables and helpers ported; loads cameos/buildup shapes via portable paths, initializes real Pointers/placement/repair logic, and sets the WEAP2 overlay hook. |
| `BUILDING.CPP` | `src/building.cpp` | Ported to src/ — full building gameplay/AI/drawing logic implemented. |
| `BULLET.CPP` | `src/bullet.cpp` | Projectile flight/fuse logic ported; keeps homing/arc/drop behaviors, shadow rendering, and explosion damage paths intact. |
| `CARGO.CPP` | `src/cargo.cpp` | Cargo hold bookkeeping ported; attach/detach preserve the chained LIFO order, while pointer coding now relies on `src/ioobj.cpp`. |
| `CCDDE.CPP` | `src/ccdde.cpp` | Ported as a portable localhost UDP implementation for launcher/lobby messaging. |
| `CCFILE.CPP` | `src/ccfile.cpp` | Mix-aware file wrapper now opens embedded mix entries (cached or on-disk) via the portable RawFile/CDFile layer. |
| `CDATA.CPP` | `src/cdata.cpp` | Ported to src/; template type tables restored (needs icon-set map helpers and viewport stamp/scale support). |
| `CDFILE.CPP` | `src/cdfile.cpp` | CD/file search helper now walks configured paths before falling back to direct opens. |
| `CELL.CPP` | `src/cell.cpp` | Partial port: core helpers implemented (constructor, lookup, redraw). |
| `COMBAT.CPP` | `src/combat.cpp` | Ported: `Modify_Damage` and `Explosion_Damage` implemented. |
| `COMBUF.CPP` | `src/combuf.cpp` | Ported core queue operations and timing helpers. |
| `COMQUEUE.CPP` | `src/comqueue.cpp` | Ported circular send/recv queue (Next/Unqueue/Get semantics) and restored `Mono_Debug_Print2` packet dump output. |
| `CONFDLG.CPP` | `src/confdlg.cpp` | Ported — confirmation dialog implementation with safe string copy. |
| `CONNECT.CPP` | `src/connect.cpp` | Ported (portable implementation; Time uses std::chrono). |
| `CONQUER.CPP` | `src/maingame.cpp` | Main_Game loop ported from the legacy file; retains SDL_QUIT push on exit while routing through the original select/loop/dialog flow, with explicit SpecialDialog cases covered. |
| `CONQUER.CPP` (source helpers) | `src/source_helpers.cpp` | `Source_From_Name`/`Name_From_Source` now follow the legacy lookup table instead of the placeholder mapping. |
| `CONST.CPP` | `src/const.cpp` | Weapon/warhead tables and coordinate helpers brought over with lowercase includes and portable tables. |
| `STARTUP.CPP` (bootstrap) | `src/port_runtime.cpp` | Init_Game/Select_Game/Main_Loop now allocate palettes/shape buffers, configure viewports, reset menu state, enable the DDE heartbeat stub, pace the frame loop using the legacy timer defaults, apply the legacy "start new game" scenario defaults when the first menu option is chosen, and make `Call_Back()` service SDL events + focus restore + blit/present for fades and modal UI loops. |
| `STARTUP.CPP` (bootstrap) | `src/port_runtime.cpp` | Initialized the global fixed heaps (`Units`, `Buildings`, `TeamTypes`, etc.) via `Set_Heap()` in `Init_Game()` so scenario loads can allocate objects (fixes crash in `TeamTypeClass::Init()` during `Clear_Scenario()`). |
| `CONTROL.CPP` | `src/control.cpp` | Control gadgets now propagate peer redraws, return KN_BUTTON IDs when triggered, and keep peers wired via a portable nullptr-safe link. |
| `COORD.CPP` | `src/coord.cpp` | Modernized coordinate helpers; `Cardinal_To_Fixed` and `Fixed_To_Cardinal` ported from `COORDA.ASM`. |
| `CREDITS.CPP` | `src/credits.cpp` | Credit counter now ticks toward the player's funds, plays up/down cues, and redraws the tab with resolution scaling. |
| `CREW.CPP` | `src/crew.cpp` | Ported (original translation unit is effectively empty; header-only helpers remain in `crew.h`). |
| `DDE.CPP` | `src/dde.cpp` | Ported as a portable localhost UDP implementation that preserves the legacy API surface. |
| `DEBUG.CPP` | `src/debug.cpp` | Partial port: `Debug_Key` and `Self_Regulate` implemented; kept feature subset for SDL input. |
| `DESCDLG.CPP` | `src/descdlg.cpp` | Ported `DescriptionClass::Process` (dialog UI with edit control and buttons). |
| `DIAL8.CPP` | `src/dial8.cpp` | Ported to src/ (gadget; hides/shows mouse, draws facing dial). |
| `DIALOG.CPP` | `src/dialog.cpp` | Ported: `Dialog_Box`, `Format_Window_String`, `Window_Box`. |
| `DISPLAY.CPP` | `src/display.cpp` | Palette tables rebuilt, fade routines wired, and display scaffolding moved to src/; pointer coding now handled in `src/iomap.cpp`. |
| `DOOR.CPP` | `src/door.cpp` | Ported to src/ (door animation state machine; open/close logic). |
| `DPMI.CPP` | `src/dpmi.cpp` | Ported to src/ with flat-memory `Swap()` implementation (no asm). |
| `DRIVE.CPP` | `src/drive.cpp` | Ported to src/ with full legacy movement logic restored; Map shim call sites use the port’s compatibility layer. |
| `ENDING.CPP` | `src/ending.cpp` | Ported: GDI/NOD ending sequences, movie playback and selection UI. |
| `EVENT.CPP` | `src/event.cpp` | Ported event constructors and execution logic, including mission assignments, production, timing updates, and special handling. |
| `EXPAND.CPP` | `src/expand.cpp` | Expansion detection now mirrors the original `EXPAND.DAT` probe so NEWMENU layouts gate off the real data file. |
| `FACING.CPP` | `src/facing.cpp` | Ported to src/ with legacy facing rotation and adjustment logic preserved. |
| `FLASHER.CPP` | `src/flasher.cpp` | Ported to src/; flash countdown toggles the blush flag and exposes mono debug output. |
| `FACTORY.CPP` | `src/factory.cpp` | Ported to src/, switched to legacy include path, and replaced NULL with nullptr. |
| `FIELD.CPP` | `src/field.cpp` | Ported to src/; moved to portable headers and retained original net byte-order conversions. |
| `FINDPATH.CPP` | `src/findpath.cpp` | Ported to src/ with pathfinding and FootClass path helpers restored. |
| `FLASHER.CPP` | | To be ported. |
| `FLY.CPP` | `src/fly.cpp` | Ported FlyClass movement/physics and speed throttle logic; removed `src/fly_stub.cpp`. |
| `FOOT.CPP` | `src/foot.cpp` | Ported to src/ with legacy movement/mission logic intact and includes updated for the SDL build. |
| `FUSE.CPP` | | To be ported. |
| `GADGET.CPP` | | To be ported. |
| `GAMEDLG.CPP` | | To be ported. |
| `GOPTIONS.CPP` | `src/goptions.cpp` | Ported to src/; options dialog flow restored (needs SDL UI verification). |
| `GSCREEN.CPP` | `src/gscreen.cpp` | Shadow-page setup and render/IO stubs recreated around modern buffers; pointer coding now handled in `src/iomap.cpp`. |
| `HDATA.CPP` | `src/hdata.cpp` | House type table migrated; colors/remap tables kept intact and Jurassic palette tweak guarded behind the Special/AreThingiesEnabled flags. |
| `HEAP.CPP` | | To be ported. |
| `HELP.CPP` | | To be ported. |
| `HOUSE.CPP` | | To be ported. |
| `IDATA.CPP` | `src/idata.cpp` | Ported to src/; infantry type tables/constructors restored (depends on icon-set map helpers). |
| `INFANTRY.CPP` | | To be ported. |
| `INI.CPP` | | To be ported. |
| `INIT.CPP` | | To be ported. |
| `INTERNET.CPP` | | To be ported. |
| `INTERPAL.CPP` | `src/interpal.cpp` | Ported to src/; interpolation palette helpers restored. |
| `INTRO.CPP` | `src/intro_port.cpp` | Minimal port; `Choose_Side()` now triggers the intro movie (VQA playback) and the full interactive flow is pending. |
| `CONQUER.CPP` (VQA playback path) | `src/movie.cpp`, `src/vqa_decoder.cpp`, `src/vqa_decoder.h` | Replaced the simulated/timing-only movie stub with a real classic (8-bit) Westwood VQA decoder (LCW/Format80) and wired `Play_Movie()` to decode frames, apply per-frame palettes, and blit into the SDL software pages. |
| `IOMAP.CPP` | `src/iomap.cpp` | Ported to src/ with pointer coding helpers for map/UI classes restored. |
| `IOOBJ.CPP` | `src/ioobj.cpp` | Ported to src/ with object save/load pointer coding and smudge/overlay helpers restored. |
| `IPX.CPP` | | To be ported. |
| `IPX95.CPP` | `src/ipx95.cpp` | Implemented the Win95 IPX95 entry points (`IPX_Send_Packet95`/`IPX_Broadcast_Packet95`/`IPX_Get_Outstanding_Buffer95`) on top of the UDP-backed `IPXConnClass`, restoring packet send/receive behavior instead of returning stub values. |
| `IPXADDR.CPP` | | To be ported. |
| `IPXCONN.CPP` | | To be ported. |
| `IPXGCONN.CPP` | | To be ported. |
| `IPXMGR.CPP` | `src/ipxmgr.cpp` | IPX manager now tracks send/receive counters and computes basic response-time stats from observed traffic, replacing placeholder return values while keeping the UDP transport layer. |
| `JSHELL.CPP` | | To be ported. |
| `KEYFRAME.CPP` | `src/keyframe_info.cpp` | Frame metadata/palette accessors ported; `Build_Frame`/LCW decode still pending. |
| `LOADDLG.CPP` | | To be ported. |
| `LOAD_TITLE.CPP` | `src/load_title.cpp` | Title screen loader now resolves art through the CCFile/Mix stack, decodes PCX or CPS deterministically, patches UI palette colors, and scales to the active viewport. Fixed PCX RLE decoding so runs that cross scanline boundaries are handled correctly (prevents main menu background corruption). |
| `LOAD_TITLE.CPP` | `src/load_title.cpp` | Title palette normalization now mirrors the legacy PCX loader and no longer overwrites the first 16 palette entries, matching Win95 background colors. |
| `LOGIC.CPP` | | To be ported. |
| `MAIN.CPP` | `src/main.cpp` | SDL bootstrap now requests a high-DPI window and nearest-neighbor scaling so UI/text pixels stay crisp. |
| `WWLIB_RUNTIME.CPP` (present blit) | `src/wwlib_runtime.cpp` | Present texture now pins SDL texture scale mode to `nearest` to avoid blurry text when SDL scales the 8-bit buffer, and ModeX_Blit forwards legacy menu blits to the SDL presenter. |
| `WWLIB_RUNTIME.CPP` (`WWMouseClass`) | `src/wwlib_runtime.cpp` | Implemented software cursor draw/erase (save-under + restore) and wired `Set_Mouse_Cursor` so legacy Hide/Show/Conditional mouse logic works. |
| `MOUSE.CPP` | `src/mouse.cpp` | Ported MouseClass cursor controller (loads `MOUSE.SHP`, supports small variants, and animates cursor frames via the legacy countdown timer). |
| `GSCREEN.CPP` | `src/gscreen.cpp` | Title screen blit texture also forces `nearest` scale mode to keep fonts/pixels sharp. |
| `GSCREEN.CPP` | `src/gscreen.cpp` | Blit_Display now routes through SeenBuff blits with mouse draw/erase hooks so SDL presentation matches the legacy flow and palette conversion stays consistent. |
| `MAP.CPP` | `src/gameplay_core_stub.cpp` | MapClass remains stubbed for now; removed duplicate Code/Decode pointer hooks in favor of `src/iomap.cpp`. |
| `MAPEDDLG.CPP` | | To be ported. |
| `MAPEDIT.CPP` | | To be ported. |
| `MAPEDPLC.CPP` | | To be ported. |
| `MAPEDSEL.CPP` | | To be ported. |
| `MAPEDTM.CPP` | | To be ported. |
| `MAPSEL.CPP` | | To be ported. |
| `MENUS.CPP` | `src/menus.cpp` | Restored the original main menu: renders the title/dialog chrome, builds the legacy button list (including expansions/bonus), pumps SDL events into the keyboard queue, honors timeouts, and returns the canonical selection indices. |
| `MISSION.CPP` | | To be ported. |
| `MIXFILE.CPP` | `src/mixfile.cpp` | Mix archive reader parses headers, caches payloads, and resolves entries by CRC for asset lookup. |
| `MIXFILE.CPP` | `src/mixfile.cpp` | Added XCC name-table support so mixed archives (e.g., CD1/CCLOCAL.MIX) with embedded filenames resolve fonts correctly. |
| `MONOC.CPP` | `src/monoc.cpp` | Ported monochrome debug buffer to a heap-backed screen page and removed DOS/segment calls while keeping the original text/box routines. |
| `MONOC.H` | `src/include/legacy/monoc.h` | Box character table now uses 8-bit storage to preserve IBM line-draw values without C++ narrowing errors. |
| `MISSION.CPP` | `src/mission.cpp` | Ported to src/ with legacy mission state machine logic intact and includes updated for the SDL build; pointer coding now handled in `src/ioobj.cpp`. |
| `MOUSE.CPP` | | To be ported. |
| `MPLAYER.CPP` | | To be ported. |
| `MSGBOX.CPP` | `src/msgbox.cpp` | Ported to src/; CCMessageBox UI logic restored. |
| `MSGLIST.CPP` | `src/msglist.cpp` | Ported to src/; MessageListClass restored. |
| `NETDLG.CPP` | | To be ported. |
| `NOSEQCON.CPP` | `src/noseqcon.cpp` | Ported non-sequenced connection queue logic into src/ with modern headers. |
| `NULLCONN.CPP` | `src/nullconn.cpp` | Ported NULL modem connection framing/CRC with UDP-backed send path. |
| `NULLDLG.CPP` | | To be ported. |
| `NULLMGR.CPP` | `src/nullmgr.cpp` | Ported NULL modem manager with UDP-based transport, queue/timing, and buffer parsing. |
| `OBJECT.CPP` | | To be ported. |
| `ODATA.CPP` | `src/odata.cpp` | Ported to src/; overlay type tables/graphics restored. |
| `OPTIONS.CPP` | `src/options.cpp` | Ported to src/; options settings and palette hooks restored, and the score-volume setter no longer recurses into itself. |
| `OVERLAY.CPP` | `src/overlay.cpp` | Ported to src/; overlay object logic restored and map-shim access updated for stubbed cells. |
| `PACKET.CPP` | | To be ported. |
| `POWER.CPP` | `src/power.cpp` | Power bar UI ported; shapes are loaded via the modern MIX helpers and redraw logic mirrors the original radar/sidebar flow. |
| `PROFILE.CPP` | | To be ported. |
| `QUEUE.CPP` | `src/queue.cpp` | Ported to src/ with mission queue helper logic and pointer coding helpers. |
| `RADAR.CPP` | | To be ported. |
| `RAWFILE.CPP` | `src/rawfile.cpp` | RawFileClass rebuilt atop POSIX read/write/seek with simple error handling. |
| `BUFFER_TO_PAGE` (legacy blit) | `src/buffer_to_page.cpp` | Raw 8-bit buffer copy now performs bounds-aware page blits instead of the stub. |
| `REINF.CPP` | `src/reinf.cpp` | Ported to src/ with reinforcement creation logic wired for triggers. |
| `SAVELOAD.CPP` | | To be ported (needed for TechnoType target helpers). |
| `SCENARIO.CPP` | | To be ported. |
| `SCORE.CPP` | `src/score.cpp` | Ported to src/; score/ending UI helpers and globals restored. |
| `SDATA.CPP` | `src/sdata.cpp` | Ported to src/; smudge type tables/graphics restored. |
| `SEQCONN.CPP` | | To be ported. |
| `SIDEBAR.CPP` | `src/sidebar.cpp` | Ported to src/; sidebar UI logic restored. |
| `SMUDGE.CPP` | `src/smudge.cpp` | Ported to src/; smudge object logic restored and map-shim access updated for stubbed cells. |
| `SOUNDDLG.CPP` | | To be ported. |
| `SPECIAL.CPP` | `src/special.cpp` | Special options dialog wired with original checkbox logic and OK/Cancel flow. |
| `STARTUP.CPP` | | To be ported. |
| `STATS.CPP` | | To be ported. |
| `SUPER.CPP` | | To be ported. |
| `TARCOM.CPP` | `src/tarcom.cpp` | Ported to src/; targeting/command logic restored. |
| `TARGET.CPP` | `src/target.cpp` | Ported to src/ with target decoding helpers for units/buildings/cells. |
| `TCPIP.CPP` | `src/tcpip.cpp` | Portable UDP-backed implementation for session messaging (PlanetWestwood globals + basic send/receive path). |
| `TDATA.CPP` | | To be ported. |
| `TEAM.CPP` | `src/team.cpp` | Ported to src/ with legacy team coordination logic and mission routing. |
| `TEAMTYPE.CPP` | `src/teamtype.cpp` | Ported to src/ with team type tables, INI parsing, and mission name helpers intact. |
| `TECHNO.CPP` | | To be ported. |
| `TEMP.CPP` | | To be ported. |
| `TEMPLATE.CPP` | `src/template.cpp` | Ported to src/; template object logic restored (depends on icon-set map helpers) and map-shim access updated. |
| `TERRAIN.CPP` | `src/terrain.cpp` | Ported to src/; terrain object logic restored. |
| `THEME.CPP` | `src/theme.cpp` | Ported to src/ with theme queueing and music selection logic preserved. |
| `TRIGGER.CPP` | `src/trigger.cpp` | Ported to src/ with trigger parsing and execution logic preserved. |
| `TURRET.CPP` | `src/turret.cpp` | Ported to src/; turret control logic restored. |
| `UDATA.CPP` | `src/udata.cpp` | Ported to src/; unit type tables/constructors restored. |
| `UNIT.CPP` | | To be ported. |
| `UTRACKER.CPP` | `src/utracker.cpp` | Unit tracker ported with network/PC byte-order conversion helpers. |
| `VISUDLG.CPP` | | To be ported. |
| `WINSTUB.CPP` | | To be ported. |
| `ABSTRACT.H` | `src/include/legacy/abstract.h` | Lowercase mirror retained so `#include "abstract.h"` works on case-sensitive hosts. |
| `COMPAT.H` | `src/include/legacy/compat.h` | Palette/buffer macros and legacy globals wrapped in portable defaults. |
| `DEFINES.H` | `src/include/legacy/defines.h` | Lowercase mirror preserving gameplay feature toggles until modernization. |
| `EXTERNS.H` | `src/include/legacy/externs.h` | Lowercase copy to keep the sprawling extern declarations accessible. |
| `FUNCTION.H` | `src/include/legacy/function.h` | Lowercase mirror ensuring the UI hierarchy declarations keep compiling; added icon-set helper declarations used by the template pipeline. |
| `MISSION.H` | `src/include/legacy/mission.h` | Lowercase copy of the mission AI declarations for case-sensitive builds. |
| `OBJECT.H` | `src/include/legacy/object.h` | Lowercase mirror safeguarding the core object hierarchy headers. |
| `REAL.H` | `src/include/legacy/real.h` | Lowercase copy retaining the original fixed-point math helpers. |
| `TARGET.H` | `src/include/legacy/target.h` | Lowercase mirror for the targeting helper declarations. |
| `TYPE.H` | `src/include/legacy/type.h` | Lowercase copy of the shared enums/typedefs used throughout the game. |
| `WWFILE.H` | `src/include/legacy/wwfile.h` | FileClass interface refreshed with `std::size_t` and a namespace alias. |
| `WWLIB32.H` | `src/include/legacy/wwlib32.h` | Navigation key constants now mirror SDL keycodes for menu/input handling; added viewport stamp/scale helpers used by template previews. |
| `WATCOM.H` | `src/include/legacy/watcom.h` | Watcom pragma wrappers swapped for GCC diagnostic helpers. |
| `PLATFORM (new)` | `src/include/legacy/platform.h` | Win16/Watcom typedef shim that turns `near`/`far` keywords into no-ops. |
| `WINDOWS_COMPAT (new)` | `src/include/legacy/windows_compat.h` | Win32 handle/struct typedef shim so the port never includes platform headers directly. |
| `CMakeLists.txt` | `CMakeLists.txt` | Added missing ported sources and gated `src/platform_win32.cpp` behind `WIN32`, then pruned duplicate stub units (`src/base_stub.cpp`, `src/gameplay_minimal_stubs.cpp`, `src/gameplay_shims.cpp`, `src/tiny_linker_shims.cpp`) to avoid duplicate symbols; `src/debug.cpp`/`src/ending.cpp` are now linked. |
| `STARTUP.CPP` (error exit) | `src/error.cpp` + `src/include/legacy/error.h` | Ported `Print_Error_End_Exit`/`Print_Error_Exit` and restored the allocation failure callbacks without dummy `printf/exit` stubs. |
| `MMX.ASM` | `src/mmx.cpp` + `src/include/legacy/mmx.h` | Replaced the MMX detection stub with a CPUID-based probe; the patch-table init is a no-op in the SDL renderer path. |
| `AIRCRAFT.H` | `src/include/legacy/aircraft.h` | Lowercase mirror retained for Linux-friendly includes. |
| `ANIM.H` | `src/include/legacy/anim.h` | Lowercase mirror retained for Linux-friendly includes. |
| `AUDIO.H` | `src/include/legacy/audio.h` | Lowercase mirror retained for Linux-friendly includes. |
| `BASE.H` | `src/include/legacy/base.h` | Lowercase mirror retained for Linux-friendly includes. |
| `BUILDING.H` | `src/include/legacy/building.h` | Lowercase mirror retained for Linux-friendly includes. |
| `BULLET.H` | `src/include/legacy/bullet.h` | Lowercase mirror retained for Linux-friendly includes. |
| `CARGO.H` | `src/include/legacy/cargo.h` | Lowercase mirror retained for Linux-friendly includes. |
| `CCDDE.H` | `src/include/legacy/ccdde.h` | Lowercase mirror retained for Linux-friendly includes. |
| `CCFILE.H` | `src/include/legacy/ccfile.h` | Lowercase mirror retained for Linux-friendly includes. |
| `CDFILE.H` | `src/include/legacy/cdfile.h` | Lowercase mirror retained for Linux-friendly includes. |
| `CELL.H` | `src/include/legacy/cell.h` | Lowercase mirror retained for Linux-friendly includes. |
| `CHECKBOX.H` | `src/include/legacy/checkbox.h` | Lowercase mirror retained for Linux-friendly includes. |
| `CHEKLIST.H` | `src/include/legacy/cheklist.h` | Lowercase mirror retained for Linux-friendly includes. |
| `COLRLIST.H` | `src/include/legacy/colrlist.h` | Lowercase mirror retained for Linux-friendly includes. |
| `COMBUF.H` | `src/include/legacy/combuf.h` | Lowercase mirror retained for Linux-friendly includes. |
| `COMQUEUE.H` | `src/include/legacy/comqueue.h` | Reuses the combuf queue entry structs to avoid duplicate typedef conflicts. |
| `CONFDLG.H` | `src/include/legacy/confdlg.h` | Lowercase mirror retained for Linux-friendly includes. |
| `CONNECT.H` | `src/include/legacy/connect.h` | Lowercase mirror retained for Linux-friendly includes. |
| `CONNMGR.H` | `src/include/legacy/connmgr.h` | Lowercase mirror retained for Linux-friendly includes. |
| `CONQUER.H` | `src/include/legacy/conquer.h` | Lowercase mirror retained for Linux-friendly includes. |
| `CONTROL.H` | `src/include/legacy/control.h` | Lowercase mirror retained for Linux-friendly includes. |
| `CREDITS.H` | `src/include/legacy/credits.h` | Lowercase mirror retained for Linux-friendly includes. |
| `CREW.H` | `src/include/legacy/crew.h` | Lowercase mirror retained for Linux-friendly includes. |
| `DDE.H` | `src/include/legacy/dde.h` | Lowercase mirror retained for Linux-friendly includes. |
| `DEBUG.H` | `src/include/legacy/debug.h` | Lowercase mirror retained for Linux-friendly includes. |
| `DESCDLG.H` | `src/include/legacy/descdlg.h` | Lowercase mirror retained for Linux-friendly includes. |
| `DIAL8.H` | `src/include/legacy/dial8.h` | Lowercase mirror retained for Linux-friendly includes. |
| `DISPLAY.H` | `src/include/legacy/display.h` | Lowercase mirror retained for Linux-friendly includes. |
| `DOOR.H` | `src/include/legacy/door.h` | Lowercase mirror retained for Linux-friendly includes. |
| `DPMI.H` | `src/include/legacy/dpmi.h` | Lowercase mirror retained for Linux-friendly includes. |
| `DRIVE.H` | `src/include/legacy/drive.h` | Lowercase mirror retained for Linux-friendly includes. |
| `EDIT.H` | `src/include/legacy/edit.h` | Lowercase mirror retained for Linux-friendly includes. |
| `ENDING.H` | `src/include/legacy/ending.h` | Lowercase mirror retained for Linux-friendly includes. |
| `EVENT.H` | `src/include/legacy/event.h` | Lowercase mirror retained for Linux-friendly includes. |
| `FACING.H` | `src/include/legacy/facing.h` | Lowercase mirror retained for Linux-friendly includes. |
| `FACTORY.H` | `src/include/legacy/factory.h` | Lowercase mirror retained for Linux-friendly includes. |
| `FIELD.H` | `src/include/legacy/field.h` | Lowercase mirror retained for Linux-friendly includes. |
| `FLASHER.H` | `src/include/legacy/flasher.h` | Lowercase mirror retained for Linux-friendly includes. |
| `FLY.H` | `src/include/legacy/fly.h` | Lowercase mirror retained for Linux-friendly includes. |
| `FOOT.H` | `src/include/legacy/foot.h` | Lowercase mirror retained for Linux-friendly includes. |
| `FUSE.H` | `src/include/legacy/fuse.h` | Lowercase mirror retained for Linux-friendly includes. |
| `GADGET.H` | `src/include/legacy/gadget.h` | Lowercase mirror retained for Linux-friendly includes. |
| `GAMEDLG.H` | `src/include/legacy/gamedlg.h` | Lowercase mirror retained for Linux-friendly includes. |
| `GAUGE.H` | `src/include/legacy/gauge.h` | Lowercase mirror retained for Linux-friendly includes. |
| `GOPTIONS.H` | `src/include/legacy/goptions.h` | Lowercase mirror retained for Linux-friendly includes. |
| `GSCREEN.H` | `src/include/legacy/gscreen.h` | Lowercase mirror retained for Linux-friendly includes. |
| `HEAP.H` | `src/include/legacy/heap.h` | Lowercase mirror retained for Linux-friendly includes. |
| `HELP.H` | `src/include/legacy/help.h` | Lowercase mirror retained for Linux-friendly includes. |
| `HOUSE.H` | `src/include/legacy/house.h` | Lowercase mirror retained for Linux-friendly includes. |
| `INFANTRY.H` | `src/include/legacy/infantry.h` | Lowercase mirror retained for Linux-friendly includes. |
| `INTRO.H` | `src/include/legacy/intro.h` | Lowercase mirror retained for Linux-friendly includes. |
| `IPX.H` | `src/include/legacy/ipx.h` | Lowercase mirror retained for Linux-friendly includes. |
| `IPX95.H` | `src/include/legacy/ipx95.h` | Lowercase mirror retained for Linux-friendly includes. |
| `IPXADDR.H` | `src/include/legacy/ipxaddr.h` | Lowercase mirror retained for Linux-friendly includes. |
| `IPXCONN.H` | `src/include/legacy/ipxconn.h` | Lowercase mirror retained for Linux-friendly includes. |
| `IPXGCONN.H` | `src/include/legacy/ipxgconn.h` | Lowercase mirror retained for Linux-friendly includes. |
| `IPXMGR.H` | `src/include/legacy/ipxmgr.h` | Lowercase mirror retained for Linux-friendly includes. |
| `JSHELL.H` | `src/include/legacy/jshell.h` | Lowercase mirror retained for Linux-friendly includes. |
| `LAYER.H` | `src/include/legacy/layer.h` | Layer container header now uses `#pragma once`, clean overrides, and forward decls. |
| `LED.H` | `src/include/legacy/led.h` | Keyboard LED helper moved to `#pragma once` and `std::uint8_t`-based control flags. |
| `LINK.H` | `src/include/legacy/link.h` | Doubly linked helper now uses `#pragma once` plus `[[nodiscard]]` accessors for list traversal. |
| `LIST.H` | `src/include/legacy/list.h` | Lowercase mirror retained for Linux-friendly includes. |
| `LOADDLG.H` | `src/include/legacy/loaddlg.h` | Lowercase mirror retained for Linux-friendly includes. |
| `LOGIC.H` | `src/include/legacy/logic.h` | Lowercase mirror retained for Linux-friendly includes. |
| `MAP.H` | `src/include/legacy/map.h` | Lowercase mirror retained for Linux-friendly includes. |
| `MAPEDIT.H` | `src/include/legacy/mapedit.h` | Lowercase mirror retained for Linux-friendly includes. |
| `MEMCHECK.H` | `src/include/legacy/memcheck.h` | Lowercase mirror retained for Linux-friendly includes. |
| `MESSAGE.H` | `src/include/legacy/message.h` | Lowercase mirror retained for Linux-friendly includes. |
| `MIXFILE.H` | `src/include/legacy/mixfile.h` | Lowercase mirror retained for Linux-friendly includes. |
| `MONOC.H` | `src/include/legacy/monoc.h` | Lowercase mirror retained for Linux-friendly includes. |
| `MOUSE.H` | `src/include/legacy/mouse.h` | Lowercase mirror retained for Linux-friendly includes. |
| `MSGBOX.H` | `src/include/legacy/msgbox.h` | Lowercase mirror retained for Linux-friendly includes. |
| `MSGLIST.H` | `src/include/legacy/msglist.h` | Lowercase mirror retained for Linux-friendly includes. |
| `NOSEQCON.H` | `src/include/legacy/noseqcon.h` | Lowercase mirror retained for Linux-friendly includes. |
| `NULLCONN.H` | `src/include/legacy/nullconn.h` | Lowercase mirror retained for Linux-friendly includes. |
| `NULLMGR.H` | `src/include/legacy/nullmgr.h` | Lowercase mirror retained for Linux-friendly includes. |
| `OPTIONS.H` | `src/include/legacy/options.h` | Lowercase mirror retained for Linux-friendly includes. |
| `OVERLAY.H` | `src/include/legacy/overlay.h` | Lowercase mirror retained for Linux-friendly includes. |
| `PACKET.H` | `src/include/legacy/packet.h` | Lowercase mirror retained for Linux-friendly includes. |
| `PHONE.H` | `src/include/legacy/phone.h` | Phone book entries now rely on `<cstring>` comparisons and zeroed buffers. |
| `POWER.H` | `src/include/legacy/power.h` | Lowercase mirror retained for Linux-friendly includes. |
| `QUEUE.H` | `src/include/legacy/queue.h` | Lowercase mirror retained for Linux-friendly includes. |
| `RADAR.H` | `src/include/legacy/radar.h` | Lowercase mirror retained for Linux-friendly includes. |
| `RADIO.H` | `src/include/legacy/radio.h` | Radio contact logic now uses `nullptr`, defaulted special members, and consistent pointer helpers. |
| `RAWFILE.H` | `src/include/legacy/rawfile.h` | Lowercase mirror retained for Linux-friendly includes. |
| `REGION.H` | `src/include/legacy/region.h` | Lowercase mirror retained for Linux-friendly includes. |
| `SAVEDLG.H` | `src/include/legacy/savedlg.h` | Lowercase mirror retained for Linux-friendly includes. |
| `SCORE.H` | `src/include/legacy/score.h` | Lowercase mirror retained for Linux-friendly includes. |
| `SCREEN.H` | `src/include/legacy/screen.h` | Lowercase mirror retained for Linux-friendly includes. |
| `SCROLL.H` | `src/include/legacy/scroll.h` | Lowercase mirror retained for Linux-friendly includes. |
| `SEQCONN.H` | `src/include/legacy/seqconn.h` | Lowercase mirror retained for Linux-friendly includes. |
| `SESSION.H` | `src/include/legacy/session.h` | Lowercase mirror retained for Linux-friendly includes. |
| `SHAPEBTN.H` | `src/include/legacy/shapebtn.h` | Lowercase mirror retained for Linux-friendly includes. |
| `SIDEBAR.H` | `src/include/legacy/sidebar.h` | Lowercase mirror retained for Linux-friendly includes. |
| `SLIDER.H` | `src/include/legacy/slider.h` | Lowercase mirror retained for Linux-friendly includes. |
| `SMUDGE.H` | `src/include/legacy/smudge.h` | Lowercase mirror retained for Linux-friendly includes. |
| `SOUNDDLG.H` | `src/include/legacy/sounddlg.h` | Lowercase mirror retained for Linux-friendly includes. |
| `SPECIAL.H` | `src/include/legacy/special.h` | Lowercase mirror retained for Linux-friendly includes. |
| `STAGE.H` | `src/include/legacy/stage.h` | Lowercase mirror retained for Linux-friendly includes. |
| `SUPER.H` | `src/include/legacy/super.h` | Lowercase mirror retained for Linux-friendly includes. |
| `TAB.H` | `src/include/legacy/tab.h` | Sidebar tab header now uses `#pragma once`, overrides, and inline helpers for redraws. |
| `TARCOM.H` | `src/include/legacy/tarcom.h` | Lowercase mirror retained for Linux-friendly includes. |
| `TCPIP.H` | `src/include/legacy/tcpip.h` | Lowercase mirror retained for Linux-friendly includes. |
| `TEAM.H` | `src/include/legacy/team.h` | Lowercase mirror retained for Linux-friendly includes. |
| `TEAMTYPE.H` | `src/include/legacy/teamtype.h` | Lowercase mirror retained for Linux-friendly includes. |
| `TECHNO.H` | `src/include/legacy/techno.h` | Lowercase mirror retained for Linux-friendly includes. |
| `TEMPLATE.H` | `src/include/legacy/template.h` | Lowercase mirror retained for Linux-friendly includes. |
| `TERRAIN.H` | `src/include/legacy/terrain.h` | Lowercase mirror retained for Linux-friendly includes. |
| `TEXTBLIT.H` | `src/include/legacy/textblit.h` | Lowercase mirror retained for Linux-friendly includes. |
| `TEXTBTN.H` | `src/include/legacy/textbtn.h` | Lowercase mirror retained for Linux-friendly includes. |
| `THEME.H` | `src/include/legacy/theme.h` | Lowercase mirror retained for Linux-friendly includes. |
| `TOGGLE.H` | `src/include/legacy/toggle.h` | Lowercase mirror retained for Linux-friendly includes. |
| `TRIGGER.H` | `src/include/legacy/trigger.h` | Lowercase mirror retained for Linux-friendly includes. |
| `TURRET.H` | `src/include/legacy/turret.h` | Lowercase mirror retained for Linux-friendly includes. |
| `TXTLABEL.H` | `src/include/legacy/txtlabel.h` | Lowercase mirror retained for Linux-friendly includes. |
| `UNIT.H` | `src/include/legacy/unit.h` | Lowercase mirror retained for Linux-friendly includes. |
| `UTRACKER.H` | `src/include/legacy/utracker.h` | Lowercase mirror retained for Linux-friendly includes. |
| `VISUDLG.H` | `src/include/legacy/visudlg.h` | Lowercase mirror retained for Linux-friendly includes. |
| `WWALLOC.H` | `src/include/legacy/wwalloc.h` | Lowercase mirror retained for Linux-friendly includes. |
| `MAP_SHIM.H` | `src/include/legacy/map_shim.h` | Map compatibility header updated with missing shim helpers/fields (cell effects + theater) needed by drive/overlay/terrain logic during the SDL bring-up. |
| `MAP_SHIM.H` | `src/include/legacy/map_shim.h` | Added missing sidebar/radar shim members plus gadget helpers for the sidebar build path. |
| `MAP_SHIM.CPP` | `src/map_shim.cpp` | Initialized radar geometry and wired shim add/remove button helpers into the shared button list. |
| `WWLIB32.H` | `src/include/legacy/wwlib32.h` | Added `KN_TAB` and `Set_Font` helper to align sidebar/font usage with SDL key constants. |
| `OPTIONS.H` | `src/include/legacy/options.h` | Added legacy `Set_Score_Vol` alias for score volume control. |
| `FUNCTION.H` | `src/include/legacy/function.h` | Declared palette helpers and RGB/HSV conversion utilities used by options and score flows. |
| `DISPLAY.CPP` | `src/display.cpp` | Added palette setter plus HSV conversion helpers to support options palette adjustments. |
| `MSGLIST.CPP` | `src/msglist.cpp` | Included the message list header so the class definitions are complete for compilation. |
| `MSGLIST.H` | `src/include/legacy/msglist.h` | Added required includes so message list types/constants resolve. |
| `TXTLABEL.H` | `src/include/legacy/txtlabel.h` | Added missing includes for gadget and text flag dependencies. |
| `WWLIB32.H` | `src/include/legacy/wwlib32.h` | Added legacy key constants and new blit/to-buffer helper declarations. |
| `WWLIB_RUNTIME.CPP` | `src/wwlib_runtime.cpp` | Implemented buffer blit overload and viewport copy helpers for message dialogs. |
| `FUNCTION.H` | `src/include/legacy/function.h` | Declared Buffer_To_Page overloads for viewports. |
| `BUFFER_TO_PAGE.CPP` | `src/buffer_to_page.cpp` | Added viewport overloads for raw buffer copies. |
| `FTIMER.H` | `src/include/legacy/ftimer.h` | Added CountDownTimerClass start helper for modal message timing. |
| `WWLIB32.H` | `src/include/legacy/wwlib32.h` | Added viewport blit overloads for message box restore paths. |
| `WWLIB_RUNTIME.CPP` | `src/wwlib_runtime.cpp` | Implemented viewport blit overload for buffer restore in dialogs. |
| `WWLIB32.H` | `src/include/legacy/wwlib32.h` | Added buffer-to-buffer blit overloads used by message box saves. |
| `WWLIB_RUNTIME.CPP` | `src/wwlib_runtime.cpp` | Implemented buffer-to-buffer blit for dialog buffer preservation. |
| `WWLIB32.H` | `src/include/legacy/wwlib32.h` | Added logic-page overloads and text print helpers for score rendering. |
| `WWLIB_RUNTIME.CPP` | `src/wwlib_runtime.cpp` | Implemented buffer-backed logic pages and buffer text printing for score flows. |
| `FUNCTION.H` | `src/include/legacy/function.h` | Declared Play_Sample for score audio hooks. |
| `AUDIO.CPP` | `src/audio.cpp` | Removed duplicate default argument on Play_Sample declaration. |
| `FUNCTION.H` | `src/include/legacy/function.h` | Added WSA animation declarations and StreamLowImpact flag used by score screens. |
| `PORT_STUBS.CPP` | `src/port_runtime.cpp` | Ported WSA/SHP drawing entry points: `Open_Animation`/`Animate_Frame`/`Get_Animation_Frame_Count`/`Close_Animation`, `Extract_Shape_Count`, and `CC_Draw_Shape` now decode and blit frames (incl. fade/translucency tables) for score/UI flows. |
| `WWLIB32.H` | `src/include/legacy/wwlib32.h` | Added buffer blit/fill helpers needed by score screen drawing. |
| `WWLIB_RUNTIME.CPP` | `src/wwlib_runtime.cpp` | Implemented buffer blit/fill helpers and Check_Key shim for score loops. |
| `FUNCTION.H` | `src/include/legacy/function.h` | Added Close_Animation/Check_Key/Extract_Shape_Count declarations for score flow. |
| `PORT_STUBS.CPP` | `src/port_runtime.cpp` | Removed placeholder Close_Animation/Extract_Shape_Count by wiring them into the real keyframe decoder. |
| `SCORE.CPP` | `src/score.cpp` | Fixed palette type and loop variable scoping for modern C++ builds. |
| `WWLIB32.H` | `src/include/legacy/wwlib32.h` | Added KN_Q/KA_TILDA constants and buffer line drawing hook. |
| `WWLIB_RUNTIME.CPP` | `src/wwlib_runtime.cpp` | Implemented buffer line drawing and Get_Key shim. |
| `FUNCTION.H` | `src/include/legacy/function.h` | Added Get_Key declaration for score name entry. |
| `SCORE.CPP` | `src/score.cpp` | Updated multi-score palettes to unsigned char to avoid narrowing errors. |
| `SCORE.CPP` | `src/score.cpp` | Aligned multi-score palette pointer types with unsigned char buffers. |
| `WWLIB32.H` | `src/include/legacy/wwlib32.h` | Added lock/offset and pitch helpers needed by palette interpolation. |
| `WWLIB_RUNTIME.CPP` | `src/wwlib_runtime.cpp` | Implemented buffer/view offsets plus lock helpers for interpolation paths. |
| `FUNCTION.H` | `src/include/legacy/function.h` | Added Wait_Blit declaration for interpolation flow. |
| `PORT_STUBS.CPP` | `src/port_runtime.cpp` | Kept `Wait_Blit` as a no-op (SDL path does not require explicit blitter waits). |
| `SMUDGE.H` | `src/include/legacy/smudge.h` | Included `wwfile.h` so FileClass I/O methods resolve. |
| `SDATA.CPP` | `src/sdata.cpp` | Added missing legacy includes for theater/mix/map helpers used by smudge setup. |
| `HELP.CPP` | `src/help.cpp` | Added HelpClass destructor definition to satisfy vtable linkage. |
| `CELL.CPP` | `src/cell.cpp` | Ported Spot_Index/Clear_Icon helpers used by infantry and radar plots. |
| `GLOBALS.CPP` | `src/globals.cpp` | Restored ModemGameToPlay global so network flows link. |
| `INTERPAL` | `src/interpal_fallback.cpp` | Added C++ palette interpolation and 2x scale fallbacks for non-ASM builds. |
| `KEYFRAME.CPP` | `src/keyframe_helpers.cpp` | Added uncompressed shape toggles and ported `Build_Frame` by implementing Westwood `Format80` (LCW) + `Format40` (XOR delta) decode for SHP/WSA frames. |
| `TEXT.CPP` | `src/text.cpp` | Added Simple_Text_Print wrapper for legacy text calls. |
| `INIT.CPP` | `src/init_helpers.cpp` | Ported Obfuscate helper for hidden option parsing. |
| `SCENARIO.CPP` | `src/scenario.cpp` | Ported scenario load/start + win/lose/restart flow so the menu can launch missions. |
| `INI.CPP` | `src/ini.cpp` | Ported scenario INI naming + parsing so `Start_Scenario()` can load mission data. |
| `SAVELOAD.CPP` | `src/saveload_helpers.cpp` | Ported TechnoType target conversions for save/load pointer coding. |
| `CONQUER.CPP` | `src/conquer_helpers.cpp` | Added selection helpers, radar icon builder, and keyboard handler for tactical input. |
| `NETDLG/NULLDLG` | `src/network_helpers.cpp` | Added network disconnect/reconnect dialog helpers for queue handling. |
| `WINSTUB` | `src/pcx_write.cpp` | Implemented PCX writer for screenshot/debug output. |
| `GAMEDLG.CPP` | `src/gamedlg.cpp` | Ported game controls dialog processing. |
| `LOADDLG.CPP` | `src/loaddlg.cpp` | Added load dialog class skeleton to satisfy build. |
| `MAP.CPP` / `LOGIC.CPP` | `src/map.cpp` / `src/logic.cpp` | Restored the original map/logic runtime and removed the linked gameplay/map stub units; the global `Map` is now a real `DisplayClass` instance again. |
| `PORT_STUBS.CPP` | `src/port_runtime.cpp` | Added global animation helper definitions for link parity. |
| `CRC` | `src/crc_helpers.cpp` | Added Calculate_CRC helper used by obfuscation and legacy CRC checks. |
| `FUNCTION.H` | `src/include/legacy/function.h` | Declared Calculate_CRC helper for obfuscation and network CRC usage. |
| `FUNCTION.H` | `src/include/legacy/function.h` | Declared Get_Shape_Header_Data helper for radar icon extraction. |
| `BITBLT` | `src/bitblt_helpers.cpp` | Added Bit_It_In_Scale fallback for score blits. |
| `SOUND/VIDEO DLG` | `src/soundvisu_helpers.cpp` | Added no-op sound/visual controls handlers to satisfy options flow. |
| `MOUSE.H` | `src/mouse_vtable.cpp` | Defined MouseClass VTable storage to satisfy serialization references. |
| `LOADDLG.H` | `src/include/legacy/loaddlg.h` | Added missing includes for vector/list/defines types. |
| `HEAP.CPP` / `COMBUF.CPP` | `src/heap.cpp`, `src/combuf.cpp` | Restored original heap Save/Load pointer coding flow and ported `CommBufferClass::Mono_Debug_Print2` so networking debug output matches the Win95 mono view again. |
| `FLY_STUB.CPP` | | `src/fly_stub.cpp` retired after porting `src/fly.cpp`; prior cleanup removed duplicate As_Movement_Coord stub to resolve linker conflicts. |
| `BUILD FIXES` | `CMakeLists.txt` | Made `TD_ENABLE_WERROR=ON` build reliably on Clang/AppleClang by suppressing legacy-warning classes while the port is in progress. |
| `BUILD FIXES` | `src/*.cpp` | Cleaned up a handful of warnings that commonly break strict builds (snprintf, signed/unsigned comparisons, missing default cases, and NULL-to-integer conversions). |
| `CCDDE.CPP` | `src/ccdde.cpp` | Replaced the `Send_Data_To_DDE_Server` stub with a portable UDP localhost implementation for launcher/lobby integration. |
| `DDE.CPP` | `src/dde.cpp` | Implemented a cross-platform DDE replacement using loopback UDP sockets (client poke + optional server bind) to preserve the legacy API surface. |
| `PORT_STUBS.CPP` | `src/port_runtime.cpp` | Removed the duplicate `Send_Data_To_DDE_Server` stub so the CCDDE implementation is authoritative. |
| Tracking | `PROGRESS.md`, `NEXT_STEPS.md`, `README.md` | Removed stale “stubbed” wording where implementations are now present (DDE/CCDDE/TCPIP/Map shim notes) and kept follow-ups scoped to incomplete subsystems. |
| `INTRO.CPP` (`Choose_Side`) | `src/intro_port.cpp` | Replaced the placeholder intro handler with a real side-selection dialog that sets `Whom` and `ScenPlayer` (GDI/Nod). |
| `DISPLAY.CPP` | `src/display.cpp` | Restored `DisplayClass::Compute_Start_Pos`/`Write_INI` implementations needed by scenario INI workflows. |
| `FUNCTION.H` | `src/include/legacy/function.h` | Declared `Invalidate_Cached_Icons` and implemented it for the SDL icon path. |
| `MPLAYER.CPP` (`Surrender_Dialog`) | `src/port_runtime.cpp` | Ported the in-game surrender confirmation dialog (OK/Cancel) and removed the unconditional “accept” stub. |
| `WWALLOC` (`Ram_Free`/`Heap_Size`) | `src/alloc.cpp` | Removed placeholder “infinite RAM” return; allocations are now tracked with a size header and `Ram_Free` reports remaining bytes based on `SDL_GetSystemRAM()`. |
| `DEBUG/STARTUP` | `src/port_debug.h` | Added verbose startup tracing gated by `TD_VERBOSE=1` / `--verbose` / `--debug`, including SDL video/render driver selection and scenario start progress logs. |

## Pending follow-ups
- Improve SDL audio mixer parity (pan/priority rules, channel reservation, fade/stop semantics) and implement streaming/music (ThemeClass).
