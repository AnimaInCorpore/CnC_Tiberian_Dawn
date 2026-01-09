# Project goal
- Port the C&C95 (Win32, flat memory model) sources with minimal churn to a portable build.
- Keep the game assets in the project root `CD/` directory, preserving existing CD subfolders; code should load directly from there.

# Platform/compatibility targets
- Build with CMake (`CMakeLists.txt`), aiming for broad g++ compatibility (C++98 era); avoid requiring compiler-specific extensions.
- Replace/encapsulate Windows/DirectX calls with SDL 1.2 equivalents so behavior matches the original rendering, audio/music playback, input, and networking flows.

# Implementation guidance
- Favor thin compatibility layers over large refactors to preserve original logic and data layouts.
- Maintain the flat Win32 memory model assumptions; avoid 16-bit/segmented code paths.
- Keep portability-focused changes localized (e.g., platform shims, SDL wrappers) to minimize touch points in core game code.
- Do not relocate or rename asset files; expect them to be read from `CD/` and its existing subdirectories.
- Keep any new code simple and do not introduce new dependencies.

# Build system expectations
- Primary entry point is CMake; avoid parallel build systems unless explicitly required.
- Default to portable SDL 1.2 usage (no SDL2) and avoid additional dependencies unless they replace DirectX functionality.

# Progress tracking
- Maintain `PROGRESS.md` as a checkbox list of source files being ported; mark entries `[x]` once they build and run correctly via the CMake + SDL 1.2 path.
- Ported sources should be added to `src/` using the same lowercase filenames as in the legacy makefile (e.g., `conquer.cpp`), and the CMake build currently produces a `cnc_td` executable.
- Follow `PORTING_RULES.md` for all porting work; keep it updated as new conventions or findings arise.
- Treat adding new findings to `PORTING_RULES.md` as mandatory whenever you discover generally important portability conventions (e.g., Watcom quirks to g++ fixes, reusable shims).
- Port each source by compiling under the target C++ standard, fix incompatibilities with minimal localized changes, and record recurring patterns or shims in `PORTING_RULES.md`.
