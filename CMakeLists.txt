cmake_minimum_required(VERSION 3.20)

project(TiberianDawn LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# The legacy code relies on GNU extensions (designated in the porting brief),
# so keep them enabled instead of forcing -std=c++14.
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(td_port STATIC
  src/CHECKBOX.cpp
  src/CHEKLIST.cpp
  src/COLRLIST.cpp
  src/GAUGE.cpp
  src/LAYER.cpp
  src/LINK.cpp
  src/MOUSEHELP.cpp
  src/RADIO.cpp
  src/RAND.cpp
  src/SCROLL.cpp
  src/SHAPEBTN.cpp
  src/SLIDER.cpp
  src/TAB.cpp
  src/TEXTBTN.cpp
  src/TOGGLE.cpp
  src/TXTLABEL.cpp
  src/VECTOR.cpp
  src/WWLIB.cpp
)

target_include_directories(td_port
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include/legacy
)

target_compile_definitions(td_port
  PRIVATE
    _WIN32
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    STRICT=1
    _CRT_SECURE_NO_WARNINGS
)

if(MSVC)
  target_compile_options(td_port PRIVATE /W4 /permissive-)
else()
  target_compile_options(td_port PRIVATE -Wall -Wextra -Wpedantic)
endif()
