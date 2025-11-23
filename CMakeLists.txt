cmake_minimum_required(VERSION 3.10)

project(CnC_Tiberian_Dawn CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- SDL2 ---
# SDL2 is the portability layer for the ported code. If SDL2 is not present
# locally, install it via your package manager before configuring.
find_package(SDL2 REQUIRED)

# --- Source Files ---
file(GLOB_RECURSE SOURCES "src/*.cpp")

# --- Executable ---
add_executable(cnc_tiberian_dawn ${SOURCES})

# --- Includes / Defines ---
target_include_directories(
  cnc_tiberian_dawn
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include/legacy
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include
    ${SDL2_INCLUDE_DIRS}
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

# --- Link Libraries ---
target_link_libraries(cnc_tiberian_dawn PRIVATE ${SDL2_LIBRARIES})
