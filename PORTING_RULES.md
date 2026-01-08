# Porting rules

- Target build: CMake + g++/C++98 (use `-std=gnu++98` if needed) producing the `cnc_td` binary.
- Source layout: place ported sources in `src/` with original lowercase filenames from the legacy makefile (e.g., `conquer.cpp`).
- Platform shims: replace/encapsulate Windows/DirectX calls with SDL 1.2 equivalents for rendering, input, audio/music, and networking; preserve original behavior.
- Assets: expect game data under `CD/` and its existing subfolders; do not relocate or rename assets.
- Memory model: keep flat Win32 assumptions; avoid 16-bit/segmented constructs.
- Change scope: prefer minimal, localized shims over broad refactors; keep data layouts intact.
- Tracking: update `PROGRESS.md` when a file builds and runs correctly through the SDL/CMake path.
- Keep this document updated with new porting conventions or findings as they emerge.
