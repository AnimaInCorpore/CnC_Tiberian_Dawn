# Porting Guide: Command & Conquer (Tiberian Dawn)

This guide directs the process of porting the original Command & Conquer source code to a modern, cross-platform build system.

## Primary Objective

Translate the legacy C++ codebase (the C&C95 Win32 build) to compile with modern tools (`g++`, `CMake`) while preserving original game logic and behavior. The end-goal is a platform-independent application using SDL for hardware abstraction.

## Core Principles

1. **Preserve Original Behavior:** The Win32 build is the canonical reference. All ported code must function identically to the original.
   - Treat the C&C95 Win32 release as the authoritative source of truth for gameplay, timing, asset handling, and UI flow.
   - Note: Rendering/audio/networking now route through SDL, and CD asset handling uses the repo-local `CD/...` mirror, so behavior in those subsystems may differ from the original Win32 implementation even when feature parity is the goal.

2. **Platform Independence:** Use SDL to replace all direct hardware calls (Graphics, Audio, Input, Networking). Avoid platform-specific code outside of the SDL implementation layer.

3. **Modern Tooling:** The codebase must build with `g++` (or a compatible compiler) and `CMake`.

4. **Clean Codebase:** Remove all legacy code paths for DOS, Win16, and segmented memory. The target is a flat 32/64-bit memory model.

5. **No Shims or Stubs:** Do not rely on shims, fallbacks, stubs, or other mockups to stand in for ported code. Fully port missing functionality to match the Win32 behavior.

6. **No New External Dependencies:** Do not introduce external dependencies beyond those explicitly allowed (e.g., SDL2/SDL_net). Implement required functionality within the port unless a specific exception is documented in this file.

7. **Minimal Divergence:** Keep the original code structure wherever possible. Only apply necessary changes—using modern best practices—to achieve platform independence and compatibility while preserving behavior.

## Porting Workflow

### 1. File Migration and Modernization

For each file (`*.CPP`, `*.H`, `*.ASM`):

1. Copy the file to `src/` directory using a lowercase filename (e.g., `AIRCRAFT.CPP` → `src/aircraft.cpp`).
2. Convert the file to a modern C++ translation unit.
3. Update include paths to reflect the new `src/` structure.
4. Replace legacy compiler specifics (e.g., Watcom pragmas, `__far`, `__near`) with standard C++ or portable wrappers.
5. Replace non-standard library calls with modern equivalents (e.g., `<cstdint>` types, `<cstring>` functions).
6. Refactor assembly code (`.ASM`) into C/C++ functions.

### 2. Dependency Replacement

#### DirectX Emulation Using SDL

To emulate DirectX features with SDL:

- **Surfaces and Rendering:** Use SDL surfaces/textures to replace DirectDraw surfaces. Implement blitting and flipping via SDL rendering functions (e.g., `SDL_RenderCopy` for drawing).
- **Palettes:** Handle palette-based graphics with SDL pixel formats and color mapping.
- **Audio Buffers:** Emulate DirectSound buffers using SDL audio queues and callbacks for mixing and playback.
- **Input Handling:** Map DirectInput axes/buttons to SDL joystick/gamepad events.
- **Networking:** Replace Greenleaf (GCL) and IPX calls with SDL_net or standard sockets.
- Ensure compatibility layers (e.g., in `src/runtime_sdl.h`) provide drop-in replacements for DirectX APIs to minimize code changes.

### 3. Build System

- Maintain a root `CMakeLists.txt`.
- As each file is ported and moved to `src/`, add it to the `CMakeLists.txt` build targets.
- The build should produce a single executable.

### 4. Verification

- The game must compile and run after each major module is ported.
- Gameplay and behavior should be frequently compared against the original Win32 version to check for regressions.
- After porting or materially updating a source/header, record the change in `PROGRESS.md` and refresh `NEXT_STEPS.md` with follow-up tasks.

## Mechanical Rules

This section defines the **mechanical, source-level rules** used while porting. Apply these rules for every ported change. If you discover a new *project-wide* mechanical rule (i.e., it should be applied consistently across many files), add it here before (or along with) applying it elsewhere.

### Baseline

- **Language standard:** ISO **C++17** (see `CMakeLists.txt`). Do not introduce C++20+ features.
- **Portability first:** avoid platform-specific code outside the platform/SDL layers.
- **Behavior first:** do not “modernize” logic; prefer minimal diffs that compile cleanly.
- Validate changes with builds, tests, and comparisons to original behavior. Use tools like `runTests` for automated checks where possible.

### Null pointers

- Use `nullptr` for null pointer constants (instead of `NULL` or integer `0`).
  - Prefer `return nullptr;` over `return NULL;` / `return(0);` when returning pointers.
- Do not pass a bare `nullptr` through **varargs** (e.g., `printf`, `sprintf`, custom `...` APIs). Cast to the expected pointer type:
  - Example: `static_cast<const char*>(nullptr)` / `static_cast<void*>(nullptr)` (as appropriate).
- Do not use `NULL` for non-pointer types; use appropriate zero values (e.g., `0` for integers).

### Booleans vs. flags

- Use `bool` / `true` / `false` for actual boolean state.
- Do not replace integer/bitmask flag code with `bool` if it participates in arithmetic or bitwise operations.

### Types and integer width

- Prefer `<cstdint>` fixed-width types (`std::uint8_t`, `std::int16_t`, …) when reading/writing **binary formats**, network payloads, file headers, or when struct layout matters.
- Avoid “fixing” warnings by changing signedness/width unless you also update the surrounding logic to keep behavior identical.
- Prefer `std::size_t` for sizes/lengths and container indices (unless the original API/ABI requires a specific type).

### Casting

- Prefer C++ casts:
  - `static_cast` for numeric and up/down-casts that are well-defined.
  - `reinterpret_cast` only when required by legacy APIs/layout (minimize its use).
- Avoid C-style casts in ported code; they hide narrowing and const-removal.
- Use `const_cast` explicitly for casting away `const`/`volatile` when required by legacy APIs (e.g., calling non-const methods on const objects). Avoid it unless necessary to preserve behavior.
  - Example: `const_cast<UnitClass*>(this)->Method()` instead of `(UnitClass*)this`.
- Use `reinterpret_cast` for pointer reinterpreting in low-level operations (e.g., byte access, buffer allocations). Always verify alignment and layout.
  - Example: `reinterpret_cast<char*>(buffer)` for raw memory manipulation.

### Const-correctness

- Preserve existing `const` usage in function parameters, return types, and variables. Do not add `const` to existing APIs unless it fixes a proven bug and doesn't break compatibility.
- When legacy code requires casting away `const`, use `const_cast` explicitly and document why (e.g., to call a non-const method).

### Headers and standard functions

- Prefer C++ headers (`<cstdint>`, `<cstring>`, `<cstdio>`, `<cstdlib>`, …) over C headers.
- Ensure the correct header is included instead of relying on incidental transitive includes.
- Prefer existing compatibility helpers (e.g., `src/include/legacy/compat.h`, `src/include/legacy/windows_compat.h`) over introducing new platform `#ifdef`s in gameplay/UI code.
- Make includes explicit and minimal; avoid relying on transitive includes from other headers.

### Struct layout and binary compatibility

- Preserve struct layout when data is persisted, serialized, or shared across modules.
- If you must change packing/alignment-related code, add a `static_assert(sizeof(Type) == expected)` where practical.

### Ownership and allocation

- Keep existing ownership/allocation patterns unless a change is required to compile or to fix a proven bug.
- Avoid introducing new lifetime semantics (e.g., widespread `std::unique_ptr` refactors) as part of mechanical porting work.

### Error handling and exceptions

- Keep existing error-handling patterns (e.g., return codes, asserts). Do not introduce C++ exceptions or `try`/`catch` blocks unless required for compilation.

### Comments and naming

- Preserve original comments, variable names, and function names. Update only if they become inaccurate due to changes.
- Follow original naming conventions (e.g., PascalCase for classes, snake_case for variables).

---

This document serves as the primary directive. Follow this plan systematically.
