cmake_minimum_required(VERSION 3.10)

project(CnC_Tiberian_Dawn LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(TD_BUILD_EXE "Build the experimental cnc_tiberian_dawn executable" ON)

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
  src/cell.cpp
  src/combuf.cpp
  src/cargo.cpp
  src/ccfile.cpp
  src/ccdde.cpp
  src/cdata.cpp
    src/comqueue.cpp
  src/cdfile.cpp
  src/checkbox.cpp
  src/dial8.cpp
  src/cheklist.cpp
  src/colrlist.cpp
  src/const.cpp
  src/control.cpp
  src/coord.cpp
  src/credits.cpp
  src/debug.cpp
  src/display.cpp
  src/edit.cpp
  src/error_stub.cpp
  src/expand.cpp
  src/factory.cpp
  src/door.cpp
  src/fly_stub.cpp
  src/gadget_control_stub.cpp
  src/game.cpp
  src/gameplay_class_stubs.cpp
  src/gameplay_core_stub.cpp
  src/gauge.cpp
  src/globals.cpp
  src/gscreen.cpp
  src/hdata.cpp
  src/heap.cpp
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
  src/link.cpp
  src/linker_stubs.cpp
  src/list.cpp
  src/load_title.cpp
  src/maingame.cpp
  src/map_shim.cpp
  src/menus.cpp
  src/intro_port.cpp
  src/movie_stub.cpp
  src/mixfile.cpp
  src/mmx_stub.cpp
  src/mousehelp.cpp
  src/name_crc.cpp
  src/object.cpp
  src/crew.cpp
  src/buffer_to_page.cpp
  src/keyframe_info.cpp
  src/platform_input.cpp
  src/port_stubs.cpp
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
  src/descdlg.cpp
  src/dialog.cpp
  src/stats.cpp
  src/super.cpp
  src/tab.cpp
  src/tcpip.cpp
  src/techno.cpp
  src/text.cpp
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

# --- Library with all ported sources ---
add_library(td_port STATIC ${TD_SOURCES})

target_include_directories(
  td_port
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include/legacy
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include
    ${TD_SDL2_INCLUDE_DIRS}
)

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

target_compile_definitions(td_port PUBLIC ${TD_COMMON_DEFS})

target_link_libraries(td_port PUBLIC ${TD_SDL2_TARGET})
if(WIN32)
  target_link_libraries(td_port PUBLIC ws2_32)
endif()

# --- Optional executable (disabled by default until more gameplay code is ported) ---
if(TD_BUILD_EXE)
  add_executable(cnc_tiberian_dawn src/main.cpp)
  target_link_libraries(cnc_tiberian_dawn PRIVATE td_port)
  if(TD_SDL2MAIN_TARGET AND NOT MINGW)
    target_link_libraries(cnc_tiberian_dawn PRIVATE ${TD_SDL2MAIN_TARGET})
  endif()
endif()

enable_testing()

add_executable(td_test_combuf src/tests/test_combuf.cpp)
target_link_libraries(td_test_combuf PRIVATE td_port)
add_test(NAME combuf_tests COMMAND td_test_combuf)

add_executable(td_test_comqueue src/tests/test_comqueue.cpp)
target_link_libraries(td_test_comqueue PRIVATE td_port)
add_test(NAME comqueue_tests COMMAND td_test_comqueue)

add_executable(td_test_dialog src/tests/test_dialog.cpp)
target_link_libraries(td_test_dialog PRIVATE td_port)
add_test(NAME dialog_tests COMMAND td_test_dialog)

add_executable(td_test_dial_and_door src/tests/test_dial_and_door.cpp)
target_link_libraries(td_test_dial_and_door PRIVATE td_port)
add_test(NAME dial_and_door_tests COMMAND td_test_dial_and_door)
