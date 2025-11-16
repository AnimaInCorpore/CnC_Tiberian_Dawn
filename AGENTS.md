# Porting Plan – Command & Conquer (Tiberian Dawn)

This document aggregates the work discussed so far for producing a modern, buildable port of the released Command & Conquer source.

## Goals
- Compile the legacy Watcom/Borland codebase with a modern GNU C++ compiler (baseline: `g++` 6.x using `-std=gnu++14`).
- Replace missing third‑party dependencies (DirectX 5 SDK, GCL, HMI/SOS) with portable equivalents suitable for desktop and eventual WebAssembly targets.
- Enable cross-platform builds via CMake and prepare for Emscripten/WebGPU/WebAudio/WebSockets backends.

## High-Level Steps
1. **Directory Layout**
   - Introduce a `src/` hierarchy and migrate each translation unit as it is modernized.
   - Keep original `*.CPP`, `*.H`, and `*.ASM` files as reference during the port.
   - Maintain filename parity inside `src/`; every original source should gain an equivalent modernized counterpart (e.g., `src/TOGGLE.cpp` mirrors `TOGGLE.CPP`) to simplify diffing and tracking progress.

2. **Modern Toolchain Bring-Up**
   - Audit the code for Watcom extensions (non‑standard pragmas, near/far keywords, inline assembly) and replace or guard them.
   - Stub or reimplement missing libraries:
     - DirectDraw/DirectSound → temporary SDL2 or similar desktop abstractions (later WebGPU/WebAudio).
     - Greenleaf Communications (GCL) networking → standard sockets/WebSockets rewrite.
     - HMI SOS audio → modern audio mixer.
   - Identify the four assembly modules required by the existing build (`WINASM`, `KEYFBUFF`, `SUPPORT`, `TXTPRNT` plus `mmx.obj`) and plan C/C++ replacements or intrinsics where practical.

3. **CMake Scaffolding**
   - Create a root `CMakeLists.txt` targeting the migrated `src/` files.
   - Provide configurable options for native desktop builds vs. Emscripten.

4. **Web Path Preparation**
   - After native builds succeed, configure Emscripten to emit WebAssembly, replacing platform APIs with:
     - WebGPU for rendering.
     - WebAudio for audio mixing.
     - DOM/Gamepad APIs for input.
     - WebSockets/WebRTC for networking.

5. **Documentation & Validation**
   - Document build prerequisites and porting status (`README`/`BUILD.md` updates).
   - Add automated builds/tests where possible to detect regressions.

## Outstanding Investigations
- Confirm which of the remaining assembly sources (e.g., `PAGFAULT.ASM`, `IPXREAL.ASM`) are still needed for special builds.
- Locate or recreate the proprietary “core engine” libraries referenced in the README (sourced from the Red Alert release).
- Determine long-term replacements for palette-based rendering and other 90s-era assumptions before finalizing the WebGPU backend.

This file should evolve as tasks are completed or new blockers are discovered.
