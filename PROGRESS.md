| Legacy file | Modern path | Notes |
| --- | --- | --- |
| `GLOBALS.CPP` | `src/globals.cpp` | Ported to src/, replaced NULL with nullptr, and disabled legacy networking code. |
| `FTIMER.H` | `src/ftimer.h` | Countdown timer helper rewritten with `#pragma once` and the global `Frame` counter. |
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
| `AADATA.CPP` | `src/aadata.cpp` | Ported to src/, replaced NULL with nullptr, `stricmp` with `strcasecmp`, `sprintf` and `_makepath` with `std::string` manipulation, and updated include path for `function.h`. |
| `ADATA.CPP` | `src/adata.cpp` | Ported to src/, updated include to the legacy shim, and replaced `_makepath` with `std::string` construction for SHP lookup. |
| `ABSTRACT.CPP` | `src/abstract.cpp` | Abstract base logic now reports the correct RTTI/ownable mask, preserves the building distance fudge, and copies INI names with `std::strncpy`. |
| `AIRCRAFT.CPP` | | To be ported. |
| `ALLOC.CPP` | | To be ported. |
| `ANIM.CPP` | | To be ported. |
| `AUDIO.CPP` | | To be ported. |
| `BASE.CPP` | | To be ported. |
| `BBDATA.CPP` | `src/bbdata.cpp` | BulletTypeClass definitions and tables ported; shape loading now uses std::string paths and the modern file helpers. |
| `BDATA.CPP` | | To be ported. |
| `BUILDING.CPP` | | To be ported. |
| `BULLET.CPP` | | To be ported. |
| `CARGO.CPP` | | To be ported. |
| `CCDDE.CPP` | | To be ported. |
| `CCFILE.CPP` | | To be ported. |
| `CDATA.CPP` | | To be ported. |
| `CDFILE.CPP` | | To be ported. |
| `CELL.CPP` | | To be ported. |
| `COMBAT.CPP` | | To be ported. |
| `COMBUF.CPP` | | To be ported. |
| `COMQUEUE.CPP` | | To be ported. |
| `CONFDLG.CPP` | | To be ported. |
| `CONNECT.CPP` | | To be ported. |
| `CONQUER.CPP` | `src/maingame.cpp` | Main_Game loop ported from the legacy file; retains SDL_QUIT push on exit while routing through the original select/loop/dialog flow. |
| `CONST.CPP` | | To be ported. |
| `CONTROL.CPP` | | To be ported. |
| `COORD.CPP` | | To be ported. |
| `CREDITS.CPP` | | To be ported. |
| `CREW.CPP` | | To be ported. |
| `DDE.CPP` | | To be ported. |
| `DEBUG.CPP` | | To be ported. |
| `DESCDLG.CPP` | | To be ported. |
| `DIAL8.CPP` | | To be ported. |
| `DIALOG.CPP` | | To be ported. |
| `DISPLAY.CPP` | `src/display.cpp` | Palette tables rebuilt, fade routines wired, and display scaffolding moved to src/. |
| `DOOR.CPP` | | To be ported. |
| `DPMI.CPP` | | To be ported. |
| `DRIVE.CPP` | | To be ported. |
| `ENDING.CPP` | | To be ported. |
| `EVENT.CPP` | | To be ported. |
| `EXPAND.CPP` | | To be ported. |
| `FACING.CPP` | | To be ported. |
| `FACTORY.CPP` | | To be ported. |
| `FIELD.CPP` | | To be ported. |
| `FINDPATH.CPP` | | To be ported. |
| `FLASHER.CPP` | | To be ported. |
| `FLY.CPP` | | To be ported. |
| `FOOT.CPP` | | To be ported. |
| `FUSE.CPP` | | To be ported. |
| `GADGET.CPP` | | To be ported. |
| `GAMEDLG.CPP` | | To be ported. |
| `GOPTIONS.CPP` | | To be ported. |
| `GSCREEN.CPP` | `src/gscreen.cpp` | Shadow-page setup and render/IO stubs recreated around modern buffers. |
| `HDATA.CPP` | | To be ported. |
| `HEAP.CPP` | | To be ported. |
| `HELP.CPP` | | To be ported. |
| `HOUSE.CPP` | | To be ported. |
| `IDATA.CPP` | | To be ported. |
| `INFANTRY.CPP` | | To be ported. |
| `INI.CPP` | | To be ported. |
| `INIT.CPP` | `src/init.cpp` | Ported to src/, replaced NULL with nullptr, and disabled platform-specific code (DOS, DDE, Winsock). |
| `INTERNET.CPP` | | To be ported. |
| `INTERPAL.CPP` | | To be ported. |
| `INTRO.CPP` | | To be ported. |
| `IOMAP.CPP` | | To be ported. |
| `IOOBJ.CPP` | | To be ported. |
| `IPX.CPP` | | To be ported. |
| `IPX95.CPP` | | To be ported. |
| `IPXADDR.CPP` | | To be ported. |
| `IPXCONN.CPP` | | To be ported. |
| `IPXGCONN.CPP` | | To be ported. |
| `IPXMGR.CPP` | `src/ipxmgr.cpp` | Simplified IPX manager stub retains global channel plumbing and connection bookkeeping. |
| `JSHELL.CPP` | | To be ported. |
| `KEYFRAME.CPP` | | To be ported. |
| `LOADDLG.CPP` | | To be ported. |
| `LOGIC.CPP` | | To be ported. |
| `MAP.CPP` | | To be ported. |
| `MAPEDDLG.CPP` | | To be ported. |
| `MAPEDIT.CPP` | | To be ported. |
| `MAPEDPLC.CPP` | | To be ported. |
| `MAPEDSEL.CPP` | | To be ported. |
| `MAPEDTM.CPP` | | To be ported. |
| `MAPSEL.CPP` | | To be ported. |
| `MENUS.CPP` | | To be ported. |
| `MISSION.CPP` | | To be ported. |
| `MIXFILE.CPP` | | To be ported. |
| `MONOC.CPP` | | To be ported. |
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
| `RAWFILE.CPP` | | To be ported. |
| `REINF.CPP` | | To be ported. |
| `SAVELOAD.CPP` | | To be ported. |
| `SCENARIO.CPP` | | To be ported. |
| `SCORE.CPP` | | To be ported. |
| `SDATA.CPP` | | To be ported. |
| `SEQCONN.CPP` | | To be ported. |
| `SIDEBAR.CPP` | | To be ported. |
| `SMUDGE.CPP` | | To be ported. |
| `SOUNDDLG.CPP` | | To be ported. |
| `SPECIAL.CPP` | | To be ported. |
| `STARTUP.CPP` | `src/startup.cpp` | Win32 entry path rebuilt with portable Win32 shims and simplified video/audio setup so the modern build can drive `Main_Game`. |
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
- None currently.
