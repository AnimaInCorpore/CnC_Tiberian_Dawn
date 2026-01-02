# Agent Porting Guide: Command & Conquer (Tiberian Dawn)

This guide directs the process of porting the original Command & Conquer source code to a modern, cross-platform build system.

## Primary Objective
Translate the legacy C++ codebase (the C&C95 Win32 build) to compile with modern tools (`g++`, `CMake`) while preserving original game logic and behavior. The end-goal is a platform-independent application using SDL for hardware abstraction.

## Core Principles
1.  **Preserve Original Behavior:** The Win32 build is the canonical reference. All ported code must function identically to the original.
    - Treat the C&C95 Win32 release as the authoritative source of truth for gameplay, timing, asset handling, and UI flow.
2.  **Maintain Parity Wherever Possible:** Keep the port aligned with the Win32 code and assets unless a deviation is required for platform compatibility, and document any unavoidable divergence.
    - Note: Rendering/audio/networking now route through SDL, and CD asset handling uses the repo-local `CD/...` mirror, so behavior in those subsystems may differ from the original Win32 implementation even when feature parity is the goal.
2.  **Platform Independence:** Use SDL to replace all direct hardware calls (Graphics, Audio, Input, Networking). Avoid platform-specific code outside of the SDL implementation layer.
3.  **Modern Tooling:** The codebase must build with `g++` (or a compatible compiler) and `CMake`.
4.  **Clean Codebase:** Remove all legacy code paths for DOS, Win16, and segmented memory. The target is a flat 32/64-bit memory model.
5.  **No Shims or Stubs:** Do not rely on shims, fallbacks, stubs, or other mockups to stand in for ported code. Fully port missing functionality to match the Win32 behavior.
6.  **No New External Dependencies:** Do not introduce external dependencies beyond those explicitly allowed here (e.g., SDL/SDL_net). Implement required functionality within the port unless a specific exception is documented in this file.
7.  **Minimal Divergence:** Keep the original code and structure wherever possible. Only apply necessary changes—using modern best practices—to achieve platform independence and compatibility while preserving behavior.

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
- After porting or materially updating a source/header, record the change in `PROGRESS.md` and refresh `NEXT_STEPS.md` with the follow-up tasks that make sense from that work.

This document serves as the primary directive. Follow this plan systematically.
