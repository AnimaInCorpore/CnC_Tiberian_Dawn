| Legacy file | Modern path | Notes |
| --- | --- | --- |
| `GLOBALS.CPP` | `src/globals.cpp` | Ported to src/, replaced NULL with nullptr, and disabled legacy networking code. |
| `FTIMER.H` | `src/include/ftimer.h` | Countdown timer helper rewritten with `#pragma once` and the global `Frame` counter. |
| `RAND.H` | `src/rand.h` | Random helper declarations cleaned up to use `<cstdint>` types. |
| `RAND.CPP` | `src/rand.cpp` | Random lookup table logic now relies on standard headers and explicit scaling. |
| `VECTOR.H` | `src/vector.h` | Vector template modernized with RAII allocation and portable guards. |
| `VECTOR.CPP` | `src/vector.cpp` | Boolean vector utilities rewritten around clear helpers and `std::memcpy`. |
| `LINK.CPP` | `src/link.cpp` | Doubly linked helper now uses `nullptr` checks and RAII-friendly removal. |
| `LAYER.CPP` | `src/layer.cpp` | Layer manager handles sorting and save/load helpers with portable pointer coding. |
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
| `ALLOC.CPP` | `src/alloc.cpp` | Legacy allocator collapsed onto the modern malloc-based wrapper (moved from the wwalloc port) while keeping the legacy entry points. |
| `ANIM.CPP` | `src/anim.cpp` | Animation system moved to src/; keeps spawn/attach logic, scorch/crater side effects, and translucent draw path intact with nullptr-safe ownership handling. |
| `AUDIO.CPP` | `src/audio.cpp` | Ported to src/; EVA/sfx logic retained and playback decision path wired. Added minimal playback stubs (`src/audio_play_stub.cpp`) — full decoder/mixer pending. |
| `BASE.CPP` | `src/base.cpp` | Ported to src/, implements base list parsing, save/load and node lookup. |
| `BBDATA.CPP` | `src/bbdata.cpp` | BulletTypeClass definitions and tables ported; shape loading now uses std::string paths and the modern file helpers. |
| `BDATA.CPP` | `src/bdata.cpp` | Building type tables and helpers ported; loads cameos/buildup shapes via portable paths, initializes real Pointers/placement/repair logic, and sets the WEAP2 overlay hook. |
| `BUILDING.CPP` | `src/building.cpp` | Ported to src/ — full building gameplay/AI/drawing logic implemented. |
| `BULLET.CPP` | `src/bullet.cpp` | Projectile flight/fuse logic ported; keeps homing/arc/drop behaviors, shadow rendering, and explosion damage paths intact. |
| `CARGO.CPP` | `src/cargo.cpp` | Cargo hold bookkeeping ported; attach/detach preserve the chained LIFO order and carry over save/load pointer coding. |
| `CCDDE.CPP` | `src/ccdde.cpp` | Ported as a stubbed DDE server (portable implementation). |
| `CCFILE.CPP` | `src/ccfile.cpp` | Mix-aware file wrapper now opens embedded mix entries (cached or on-disk) via the portable RawFile/CDFile layer. |
| `CDATA.CPP` | `src/cdata.cpp` | Stub added — full port pending. |
| `CDFILE.CPP` | `src/cdfile.cpp` | CD/file search helper now walks configured paths before falling back to direct opens. |
| `CELL.CPP` | `src/cell.cpp` | Partial port: core helpers implemented (constructor, lookup, redraw). |
| `COMBAT.CPP` | `src/combat.cpp` | Ported: `Modify_Damage` and `Explosion_Damage` implemented. |
| `COMBUF.CPP` | `src/combuf.cpp` | Ported core queue operations and timing helpers. |
| `COMQUEUE.CPP` | `src/comqueue.cpp` | Ported circular send/recv queue (Next/Unqueue/Get semantics). |
| `CONFDLG.CPP` | `src/confdlg.cpp` | Ported — confirmation dialog implementation with safe string copy. |
| `CONNECT.CPP` | `src/connect.cpp` | Ported (portable implementation; Time uses std::chrono). |
| `CONQUER.CPP` | `src/maingame.cpp` | Main_Game loop ported from the legacy file; retains SDL_QUIT push on exit while routing through the original select/loop/dialog flow, with explicit SpecialDialog cases covered. |
| `CONQUER.CPP` (source helpers) | `src/source_helpers.cpp` | `Source_From_Name`/`Name_From_Source` now follow the legacy lookup table instead of the placeholder mapping. |
| `CONST.CPP` | `src/const.cpp` | Weapon/warhead tables and coordinate helpers brought over with lowercase includes and portable tables. |
| `STARTUP.CPP` (bootstrap) | `src/port_stubs.cpp` | Init_Game/Select_Game/Main_Loop now allocate palettes/shape buffers, configure viewports, reset menu state, enable the DDE heartbeat stub, and pace the frame loop using the legacy timer defaults. |
| `CONTROL.CPP` | `src/control.cpp` | Control gadgets now propagate peer redraws, return KN_BUTTON IDs when triggered, and keep peers wired via a portable nullptr-safe link. |
| `COORD.CPP` | `src/coord.cpp` | Modernized coordinate helpers; `Cardinal_To_Fixed` and `Fixed_To_Cardinal` ported from `COORDA.ASM`. |
| `CREDITS.CPP` | `src/credits.cpp` | Credit counter now ticks toward the player's funds, plays up/down cues, and redraws the tab with resolution scaling. |
| `CREW.CPP` | `src/crew.cpp` | Ported (minimal stub; class implemented in header). |
| `DDE.CPP` | `src/dde.cpp` | Ported (portable stub of DDE instance class). |
| `DEBUG.CPP` | `src/debug.cpp` | Partial port: `Debug_Key` and `Self_Regulate` implemented; kept feature subset for SDL input. |
| `DESCDLG.CPP` | `src/descdlg.cpp` | Ported `DescriptionClass::Process` (dialog UI with edit control and buttons). |
| `DIAL8.CPP` | `src/dial8.cpp` | Ported to src/ (gadget; hides/shows mouse, draws facing dial). |
| `DIALOG.CPP` | `src/dialog.cpp` | Ported: `Dialog_Box`, `Format_Window_String`, `Window_Box`. |
| `DISPLAY.CPP` | `src/display.cpp` | Palette tables rebuilt, fade routines wired, and display scaffolding moved to src/. |
| `DOOR.CPP` | `src/door.cpp` | Ported to src/ (door animation state machine; open/close logic). |
| `DPMI.CPP` | `src/dpmi.cpp` | Ported to src/ with flat-memory `Swap()` implementation (no asm). |
| `DRIVE.CPP` | | To be ported. |
| `ENDING.CPP` | `src/ending.cpp` | Ported: GDI/NOD ending sequences, movie playback and selection UI. |
| `EVENT.CPP` | | To be ported. |
| `EXPAND.CPP` | `src/expand.cpp` | Expansion detection now mirrors the original `EXPAND.DAT` probe so NEWMENU layouts gate off the real data file. |
| `FACING.CPP` | `src/facing.cpp` | Ported to src/ with legacy facing rotation and adjustment logic preserved. |
| `FLASHER.CPP` | `src/flasher.cpp` | Ported to src/; flash countdown toggles the blush flag and exposes mono debug output. |
| `FACTORY.CPP` | | To be ported. |
| `FIELD.CPP` | `src/field.cpp` | Ported to src/; moved to portable headers and retained original net byte-order conversions. |
| `FINDPATH.CPP` | | To be ported. |
| `FLASHER.CPP` | | To be ported. |
| `FLY.CPP` | | To be ported. |
| `FOOT.CPP` | | To be ported. |
| `FUSE.CPP` | | To be ported. |
| `GADGET.CPP` | | To be ported. |
| `GAMEDLG.CPP` | | To be ported. |
| `GOPTIONS.CPP` | | To be ported. |
| `GSCREEN.CPP` | `src/gscreen.cpp` | Shadow-page setup and render/IO stubs recreated around modern buffers. |
| `HDATA.CPP` | `src/hdata.cpp` | House type table migrated; colors/remap tables kept intact and Jurassic palette tweak guarded behind the Special/AreThingiesEnabled flags. |
| `HEAP.CPP` | | To be ported. |
| `HELP.CPP` | | To be ported. |
| `HOUSE.CPP` | | To be ported. |
| `IDATA.CPP` | | To be ported. |
| `INFANTRY.CPP` | | To be ported. |
| `INI.CPP` | | To be ported. |
| `INIT.CPP` | | To be ported. |
| `INTERNET.CPP` | | To be ported. |
| `INTERPAL.CPP` | | To be ported. |
| `INTRO.CPP` | `src/intro_port.cpp` | Minimal stub ported; `Choose_Side()` now triggers the intro movie (stub playback). Full VQA/interactive flow pending. |
| `IOMAP.CPP` | | To be ported. |
| `IOOBJ.CPP` | | To be ported. |
| `IPX.CPP` | | To be ported. |
| `IPX95.CPP` | | To be ported. |
| `IPXADDR.CPP` | | To be ported. |
| `IPXCONN.CPP` | | To be ported. |
| `IPXGCONN.CPP` | | To be ported. |
| `IPXMGR.CPP` | `src/ipxmgr.cpp` | Simplified IPX manager stub retains global channel plumbing and connection bookkeeping. |
| `JSHELL.CPP` | | To be ported. |
| `KEYFRAME.CPP` | `src/keyframe_info.cpp` | Frame metadata/palette accessors ported; `Build_Frame`/LCW decode still pending. |
| `LOADDLG.CPP` | | To be ported. |
| `LOAD_TITLE.CPP` | `src/load_title.cpp` | Title screen loader now resolves art through the CCFile/Mix stack, decodes PCX or CPS deterministically, patches UI palette colors, and scales to the active viewport. Fixed PCX RLE decoding so runs that cross scanline boundaries are handled correctly (prevents main menu background corruption). |
| `LOGIC.CPP` | | To be ported. |
| `MAIN.CPP` | `src/main.cpp` | SDL bootstrap now requests a high-DPI window and nearest-neighbor scaling so UI/text pixels stay crisp. |
| `WWLIB_RUNTIME.CPP` (present blit) | `src/wwlib_runtime.cpp` | Present texture now pins SDL texture scale mode to `nearest` to avoid blurry text when SDL scales the 8-bit buffer, and ModeX_Blit forwards legacy menu blits to the SDL presenter. |
| `GSCREEN.CPP` | `src/gscreen.cpp` | Title screen blit texture also forces `nearest` scale mode to keep fonts/pixels sharp. |
| `MAP.CPP` | | To be ported. |
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
| `MOUSE.CPP` | | To be ported. |
| `MPLAYER.CPP` | | To be ported. |
| `MSGBOX.CPP` | | To be ported. |
| `MSGLIST.CPP` | | To be ported. |
| `NETDLG.CPP` | | To be ported. |
| `NOSEQCON.CPP` | | To be ported. |
| `NULLCONN.CPP` | | To be ported. |
| `NULLDLG.CPP` | | To be ported. |
| `NULLMGR.CPP` | | To be ported. |
| `OBJECT.CPP` | | To be ported. |
| `ODATA.CPP` | | To be ported. |
| `OPTIONS.CPP` | | To be ported. |
| `OVERLAY.CPP` | | To be ported. |
| `PACKET.CPP` | | To be ported. |
| `POWER.CPP` | `src/power.cpp` | Power bar UI ported; shapes are loaded via the modern MIX helpers and redraw logic mirrors the original radar/sidebar flow. |
| `PROFILE.CPP` | | To be ported. |
| `QUEUE.CPP` | | To be ported. |
| `RADAR.CPP` | | To be ported. |
| `RAWFILE.CPP` | `src/rawfile.cpp` | RawFileClass rebuilt atop POSIX read/write/seek with simple error handling. |
| `BUFFER_TO_PAGE` (legacy blit) | `src/buffer_to_page.cpp` | Raw 8-bit buffer copy now performs bounds-aware page blits instead of the stub. |
| `REINF.CPP` | | To be ported. |
| `SAVELOAD.CPP` | | To be ported. |
| `SCENARIO.CPP` | | To be ported. |
| `SCORE.CPP` | | To be ported. |
| `SDATA.CPP` | | To be ported. |
| `SEQCONN.CPP` | | To be ported. |
| `SIDEBAR.CPP` | | To be ported. |
| `SMUDGE.CPP` | | To be ported. |
| `SOUNDDLG.CPP` | | To be ported. |
| `SPECIAL.CPP` | `src/special.cpp` | Special options dialog wired with original checkbox logic and OK/Cancel flow. |
| `STARTUP.CPP` | | To be ported. |
| `STATS.CPP` | | To be ported. |
| `SUPER.CPP` | | To be ported. |
| `TARCOM.CPP` | | To be ported. |
| `TARGET.CPP` | | To be ported. |
| `TCPIP.CPP` | `src/tcpip.cpp` | Winsock shim stub tracks connection state, buffers, and PlanetWestwood globals. |
| `TDATA.CPP` | | To be ported. |
| `TEAM.CPP` | | To be ported. |
| `TEAMTYPE.CPP` | | To be ported. |
| `TECHNO.CPP` | | To be ported. |
| `TEMP.CPP` | | To be ported. |
| `TEMPLATE.CPP` | | To be ported. |
| `TERRAIN.CPP` | | To be ported. |
| `THEME.CPP` | | To be ported. |
| `TRIGGER.CPP` | | To be ported. |
| `TURRET.CPP` | | To be ported. |
| `UDATA.CPP` | | To be ported. |
| `UNIT.CPP` | | To be ported. |
| `UTRACKER.CPP` | `src/utracker.cpp` | Unit tracker ported with network/PC byte-order conversion helpers. |
| `VISUDLG.CPP` | | To be ported. |
| `WINSTUB.CPP` | | To be ported. |
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
| `WWLIB32.H` | `src/include/legacy/wwlib32.h` | Navigation key constants now mirror SDL keycodes for menu/input handling. |
| `WATCOM.H` | `src/include/legacy/watcom.h` | Watcom pragma wrappers swapped for GCC diagnostic helpers. |
| `PLATFORM (new)` | `src/include/legacy/platform.h` | Win16/Watcom typedef shim that turns `near`/`far` keywords into no-ops. |
| `WINDOWS_COMPAT (new)` | `src/include/legacy/windows_compat.h` | Win32 handle/struct typedef shim so the port never includes platform headers directly. |
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
| `COMQUEUE.H` | `src/include/legacy/comqueue.h` | Lowercase mirror retained for Linux-friendly includes. |
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

## Pending follow-ups
- Implement audio decoder/mixer (decode .AUD/.JUV/.Vxx and mix via SDL; add panning/priority and voice queue)
