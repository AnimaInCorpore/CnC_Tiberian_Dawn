cmake_minimum_required(VERSION 3.10)

project(CnC_Tiberian_Dawn CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(TD_BUILD_EXE "Build the experimental cnc_tiberian_dawn executable" ON)
option(TD_ENABLE_WINMAIN "Build the legacy WinMain launcher (Windows only)" OFF)

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
file(GLOB_RECURSE SOURCES "src/*.cpp")
# Keep the Win32 launcher opt-in so the default build stays SDL-only.
list(FILTER SOURCES EXCLUDE REGEX ".*/platform_win32\\.cpp$")
if(TD_ENABLE_WINMAIN AND WIN32)
  list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/platform_win32.cpp)
endif()

# --- Library with all ported sources ---
add_library(td_port STATIC ${SOURCES})

target_include_directories(
  td_port
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include/legacy
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include
    ${TD_SDL2_INCLUDE_DIRS}
)

target_compile_definitions(
  td_port
  PRIVATE
    NOMINMAX
    STRICT=1
    TD_PORT_USE_SDL2
    WIN32_LEAN_AND_MEAN
    _CRT_SECURE_NO_WARNINGS
    SCENARIO_EDITOR
)

if(WIN32)
  target_link_libraries(td_port PUBLIC ws2_32)
endif()
target_link_libraries(td_port PUBLIC ${TD_SDL2_TARGET})

# --- Optional executable (disabled by default until more gameplay code is ported) ---
if(TD_BUILD_EXE)
  add_executable(cnc_tiberian_dawn src/main.cpp)
  target_include_directories(
    cnc_tiberian_dawn
    PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/src
      ${CMAKE_CURRENT_SOURCE_DIR}/src/include/legacy
      ${CMAKE_CURRENT_SOURCE_DIR}/src/include
      ${TD_SDL2_INCLUDE_DIRS}
  )
  target_compile_definitions(
    cnc_tiberian_dawn
    PRIVATE
      NOMINMAX
      STRICT=1
      TD_PORT_USE_SDL2
      WIN32_LEAN_AND_MEAN
      _CRT_SECURE_NO_WARNINGS
      SCENARIO_EDITOR
  )
  target_link_libraries(cnc_tiberian_dawn PRIVATE td_port ${TD_SDL2_TARGET})
  if(TD_SDL2MAIN_TARGET AND NOT MINGW)
    target_link_libraries(cnc_tiberian_dawn PRIVATE ${TD_SDL2MAIN_TARGET})
  endif()
endif()
