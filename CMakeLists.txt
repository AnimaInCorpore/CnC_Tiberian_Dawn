cmake_minimum_required(VERSION 3.16)

project(CnC_Tiberian_Dawn LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

file(GLOB_RECURSE CNC_TD_SOURCES CONFIGURE_DEPENDS
    src/*.c
    src/*.cc
    src/*.cpp
)

if(NOT CNC_TD_SOURCES)
    message(STATUS "No sources found under src/; using stub entry point.")
endif()


add_executable(cnc_td ${CNC_TD_SOURCES})

target_include_directories(cnc_td PRIVATE src)

# SDL 1.2 (portable replacement for Win32/DirectX services).
# Prefer CMake's FindSDL module when available; fall back to pkg-config.
find_package(SDL QUIET)
if(SDL_FOUND)
    target_include_directories(cnc_td PRIVATE ${SDL_INCLUDE_DIR})
    target_link_libraries(cnc_td PRIVATE ${SDL_LIBRARY})
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL12 REQUIRED sdl)
    target_include_directories(cnc_td PRIVATE ${SDL12_INCLUDE_DIRS})
    target_link_libraries(cnc_td PRIVATE ${SDL12_LIBRARIES})
endif()
