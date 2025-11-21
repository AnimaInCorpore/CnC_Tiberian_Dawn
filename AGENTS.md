# Agent Porting Guide: Command & Conquer (Tiberian Dawn)

This guide directs the process of porting the original Command & Conquer source code to a modern, cross-platform build system.

## Primary Objective
Translate the legacy C++ codebase to compile with modern tools (`g++`, `CMake`) while preserving original game logic and behavior. The end-goal is a platform-independent application using SDL for hardware abstraction.

## Core Principles
1.  **Preserve Original Behavior:** The Win32 build is the canonical reference. All ported code must function identically to the original.
2.  **Platform Independence:** Use SDL to replace all direct hardware calls (Graphics, Audio, Input, Networking). Avoid platform-specific code outside of the SDL implementation layer.
3.  **Modern Tooling:** The codebase must build with `g++` (or a compatible compiler) and `CMake`.
4.  **Clean Codebase:** Remove all legacy code paths for DOS, Win16, and segmented memory. The target is a flat 32/64-bit memory model.

## Porting Workflow

### 1. File Migration and Modernization
- For each file (`*.CPP`, `*.H`, `*.ASM`):
    1.  Copy the file to a `src/` directory, using a lowercase filename (e.g., `AIRCRAFT.CPP` -> `src/aircraft.cpp`).
    2.  Convert the file to a modern C++ translation unit.
    3.  Update include paths to reflect the new `src/` structure.
    4.  Replace legacy compiler specifics (e.g., Watcom pragmas, `__far`, `__near`) with standard C++ or portable wrappers.
    5.  Replace non-standard library calls with modern equivalents (e.g., `<cstdint>` types, `<cstring>` functions).
    6.  Refactor assembly code (`.ASM`) into C/C++ functions.

### 2. Dependency Replacement
- **Graphics:** Replace all DirectDraw calls with the SDL2 rendering API.
- **Audio:** Replace DirectSound and HMI/SOS calls with the SDL2 audio API.
- **Networking:** Replace Greenleaf (GCL) and IPX calls with SDL_net or standard sockets.
- **Input:** Replace DirectInput and low-level keyboard/mouse hooks with the SDL2 event system.

### 3. Build System
- Maintain a root `CMakeLists.txt`.
- As each file is ported and moved to `src/`, add it to the `CMakeLists.txt` build targets.
- The build should produce a single executable.

### 4. Verification
- The game must compile and run after each major module is ported.
- Gameplay and behavior should be frequently compared against the original Win32 version to check for regressions.

## Task Checklist (High-Level)
- [ ] **Setup:** Create `src/` directory and `CMakeLists.txt`.
- [ ] **Headers:** Port all `.H` files to `src/include/`, modernizing types and removing legacy conditionals.
- [ ] **Core Logic:** Port `.CPP` files, starting with those with fewest dependencies.
- [ ] **Assembly:** Re-implement all `.ASM` files in C++.
- [ ] **SDL Integration:** Implement graphics, audio, and input backends using SDL.
- [ ] **Build & Test:** Continuously build and test the application.

This document serves as the primary directive. Follow this plan systematically.