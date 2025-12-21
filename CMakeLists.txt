cmake_minimum_required(VERSION 3.10)

project(CnC_Tiberian_Dawn LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(TD_ENABLE_WERROR "Treat compiler warnings as errors (for CI/debug)" OFF)

# --- SDL2 ---
# SDL2 is the portability layer for the ported code. If SDL2 is not present
# locally, install it via your package manager before configuring.
set(SDL2_STATIC ON) # Prefer the static SDL2 target if available
find_package(SDL2 REQUIRED)

# Handle SDL2 target selection across the different package styles (config vs. module, static vs. shared).
if(TARGET SDL2::SDL2-static)
  set(TD_SDL2_TARGET SDL2::SDL2-static)
elseif(TARGET SDL2::SDL2)
  set(TD_SDL2_TARGET SDL2::SDL2)
elseif(DEFINED SDL2_LIBRARIES)
  set(TD_SDL2_TARGET ${SDL2_LIBRARIES})
else()
  message(FATAL_ERROR "SDL2 was found, but no usable CMake target was exported. Please update/repair your SDL2 installation.")
endif()

if(TARGET SDL2::SDL2main)
  set(TD_SDL2MAIN_TARGET SDL2::SDL2main)
elseif(DEFINED SDL2_MAIN_LIBRARY)
  set(TD_SDL2MAIN_TARGET ${SDL2_MAIN_LIBRARY})
elseif(DEFINED SDL2MAIN_LIBRARY)
  set(TD_SDL2MAIN_TARGET ${SDL2MAIN_LIBRARY})
endif()

set(TD_SDL2_INCLUDE_DIRS ${SDL2_INCLUDE_DIRS})
if(NOT TD_SDL2_INCLUDE_DIRS AND DEFINED SDL2_INCLUDE_DIR)
  set(TD_SDL2_INCLUDE_DIRS ${SDL2_INCLUDE_DIR})
endif()

# --- Source Files ---
set(TD_SOURCES
  src/aadata.cpp
  src/abstract.cpp
  src/adata.cpp
  src/aircraft.cpp
  src/anim.cpp
  src/alloc.cpp
  src/audio_stub.cpp
  src/audio.cpp
  src/audio_play_stub.cpp
  src/base.cpp
  src/bbdata.cpp
  src/bdata.cpp
  src/building.cpp
  src/combat.cpp
  src/bullet.cpp
  src/fuse.cpp
  src/foot.cpp
  src/flasher.cpp
  src/field.cpp
  src/cell.cpp
  src/combuf.cpp
  src/cargo.cpp
  src/ccfile.cpp
  src/ccdde.cpp
  src/cdata.cpp
  src/icon_set.cpp
  src/comqueue.cpp
  src/cdfile.cpp
  src/checkbox.cpp
  src/dial8.cpp
  src/cheklist.cpp
  src/colrlist.cpp
  src/const.cpp
  src/control.cpp
  src/coord.cpp
  src/facing.cpp
  src/credits.cpp
  src/debug.cpp
  src/display.cpp
  src/edit.cpp
  src/error_stub.cpp
  src/ending.cpp
  src/expand.cpp
  src/factory.cpp
  src/drive.cpp
  src/door.cpp
  src/fly.cpp
  src/overlay.cpp
  src/smudge.cpp
  src/terrain.cpp
  src/template.cpp
  src/tarcom.cpp
  src/turret.cpp
  src/sidebar.cpp
  src/options.cpp
  src/goptions.cpp
  src/msglist.cpp
  src/msgbox.cpp
  src/score.cpp
  src/interpal.cpp
  src/interpal_fallback.cpp
  src/udata.cpp
  src/idata.cpp
  src/odata.cpp
  src/sdata.cpp
  src/cdata.cpp
  src/tdata.cpp
  src/keyframe_helpers.cpp
  src/init_helpers.cpp
  src/scenario_helpers.cpp
  src/ini_helpers.cpp
  src/saveload_helpers.cpp
  src/conquer_helpers.cpp
  src/network_helpers.cpp
  src/pcx_write.cpp
  src/crc_helpers.cpp
  src/bitblt_helpers.cpp
  src/soundvisu_helpers.cpp
  src/mouse_vtable.cpp
  src/gamedlg.cpp
  src/loaddlg.cpp
  
  src/gadget_control_stub.cpp
  src/game.cpp
  src/gauge.cpp
  src/globals.cpp
  src/gscreen.cpp
  src/hdata.cpp
  src/heap.cpp
  src/help.cpp
  src/dpmi.cpp
  src/house.cpp
  src/infantry.cpp
  src/input_shim.cpp
  src/ipx.cpp
  src/ipx95.cpp
  src/ipxaddr.cpp
  src/ipxconn.cpp
  src/connect.cpp
  src/ipxgconn.cpp
  src/ipxmgr.cpp
  src/layer.cpp
  src/legacy_vtables.cpp
  src/gameplay_core_stub.cpp
  # src/gameplay_class_stubs.cpp (excluded: duplicates real implementations)
  # src/linker_stubs.cpp (excluded: duplicates real implementations)
  src/port_stubs.cpp
  src/link.cpp
  src/list.cpp
  # SDATA.CPP
  # IDATA.CPP
  # CDATA.CPP
  src/load_title.cpp
  src/maingame.cpp
  src/map_shim.cpp
  src/menus.cpp
  src/mission.cpp
  src/intro_port.cpp
  src/movie_stub.cpp
  src/mixfile.cpp
  src/mmx_stub.cpp
  src/monoc.cpp
  src/mousehelp.cpp
  src/name_crc.cpp
  src/noseqcon.cpp
  src/nullconn.cpp
  src/nullmgr.cpp
  src/object.cpp
  src/crew.cpp
  src/buffer_to_page.cpp
  src/keyframe_info.cpp
  src/platform_input.cpp
  src/power.cpp
  src/radar.cpp
  src/radio.cpp
  src/rand.cpp
  src/rawfile.cpp
  src/saveload_utils.cpp
  src/dde.cpp
  src/runtime_sdl.cpp
  src/scroll.cpp
  src/source_helpers.cpp
  src/shapebtn.cpp
  src/slider.cpp
  src/special.cpp
  src/confdlg.cpp
  src/strtrim.cpp
  src/window.cpp
  src/descdlg.cpp
  src/dialog.cpp
  src/event.cpp
  src/stats.cpp
  src/super.cpp
  src/tab.cpp
  src/tcpip.cpp
  src/techno.cpp
  src/team.cpp
  src/teamtype.cpp
  src/text.cpp
  src/trigger.cpp
  src/theme.cpp
  src/target.cpp
  src/reinf.cpp
  src/queue.cpp
  src/iomap.cpp
  src/ioobj.cpp
  src/findpath.cpp
  src/txtprnt.cpp
  src/textbtn.cpp
  src/toggle.cpp
  src/txtlabel.cpp
  src/unit.cpp
  src/utracker.cpp
  src/vector.cpp
  src/wwlib_runtime.cpp
  src/wwlib.cpp
)

# Legacy WinMain launcher intentionally excluded to keep the SDL path canonical.

set(TD_COMMON_DEFS
  TD_PORT_USE_SDL2
  SCENARIO_EDITOR
)
if(WIN32)
  list(APPEND TD_COMMON_DEFS
    NOMINMAX
    STRICT=1
    WIN32_LEAN_AND_MEAN
    _CRT_SECURE_NO_WARNINGS
  )
endif()

add_executable(cnc_tiberian_dawn src/main.cpp ${TD_SOURCES})

if(WIN32)
  target_sources(cnc_tiberian_dawn PRIVATE src/platform_win32.cpp)
endif()

target_include_directories(
  cnc_tiberian_dawn
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include/legacy
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include
    ${TD_SDL2_INCLUDE_DIRS}
)

target_compile_definitions(cnc_tiberian_dawn PRIVATE ${TD_COMMON_DEFS})

if(TD_ENABLE_WERROR)
  if(CMAKE_CXX_COMPILER_ID MATCHES "^(Clang|GNU|AppleClang)$")
    target_compile_options(cnc_tiberian_dawn PRIVATE -Wall -Wextra -Werror
      -Wno-char-subscripts
      -Wno-deprecated-declarations
      -Wno-format
      -Wno-missing-braces
      -Wno-null-conversion
      -Wno-logical-op-parentheses
      -Wno-sign-compare
      -Wno-switch
      -Wno-unknown-pragmas
      -Wno-writable-strings
      -Wno-unused-but-set-variable
      -Wno-overloaded-virtual
      -Wno-self-assign
      -Wno-delete-incomplete
      -Wno-reorder-ctor
      -Wno-sometimes-uninitialized
      -Wno-unused-function
      -Wno-unused-variable
      -Wno-uninitialized
      -Wno-unused-parameter)
  endif()
endif()

target_link_libraries(cnc_tiberian_dawn PRIVATE ${TD_SDL2_TARGET})
if(TD_SDL2MAIN_TARGET AND NOT MINGW)
  target_link_libraries(cnc_tiberian_dawn PRIVATE ${TD_SDL2MAIN_TARGET})
endif()
if(WIN32)
  target_link_libraries(cnc_tiberian_dawn PRIVATE ws2_32)
endif()
