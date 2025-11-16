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
   - Preserve the original data type widths (e.g., 8/16/32-bit ints) when refactoring; matching the legacy assumptions is critical for eventual backports to low-memory or 16-bit-oriented systems like Amiga/Atari ST.
   - Stub or reimplement missing libraries:
     - DirectDraw/DirectSound → SDL2 rendering/audio backends for portable desktop builds.
     - Greenleaf Communications (GCL) networking → SDL_net or standard sockets.
     - HMI SOS audio → modern audio mixer.
   - Identify the four assembly modules required by the existing build (`WINASM`, `KEYFBUFF`, `SUPPORT`, `TXTPRNT` plus `mmx.obj`) and plan C/C++ replacements or intrinsics where practical.

3. **CMake Scaffolding**
   - Create a root `CMakeLists.txt` targeting the migrated `src/` files.
   - Provide configurable options for native desktop builds vs. Emscripten.

4. **Web Path Preparation**
   - After the SDL-backed native build succeeds, configure Emscripten to emit WebAssembly.
   - Use SDL’s Emscripten support as the portability layer, or swap in WebGPU/WebAudio/WebSockets equivalents if higher performance or finer control is required.

5. **Documentation & Validation**
   - Document build prerequisites and porting status (`README`/`BUILD.md` updates).
   - Add automated builds/tests where possible to detect regressions.

## Header Modernization Strategy
- Prioritize porting umbrella headers (e.g., `COMPAT.H`, `DEFINES.H`, `TYPE.H`, `REAL.H`, `WATCOM.H`, `WWFILE.H`, `FUNCTION.H`, `EXTERNS.H`) into `src/include` before touching many translation units.
- Replace Watcom/Borland-specific types (`WORD`, `movmem`, `near`, `i86.h`) with `<cstdint>` typedefs, guarded legacy constants, and portable helpers.
- Mirror original filenames inside the new include tree so every legacy `*.H` has a modern counterpart and diffing stays simple.
- Once the shared headers compile, migrate translation units that only depend on the ported interfaces to reduce churn.

## Outstanding Investigations
- Confirm which of the remaining assembly sources (e.g., `PAGFAULT.ASM`, `IPXREAL.ASM`) are still needed for special builds.
- Locate or recreate the proprietary “core engine” libraries referenced in the README (sourced from the Red Alert release).
- Determine long-term replacements for palette-based rendering and other 90s-era assumptions before finalizing the WebGPU backend.

This file should evolve as tasks are completed or new blockers are discovered.
