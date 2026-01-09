# Porting rules

- Target build: CMake + g++/C++98 (use `-std=gnu++98` if needed) producing the `cnc_td` binary.
- Source layout: place ported sources in `src/` with original lowercase filenames from the legacy makefile (e.g., `conquer.cpp`).
- Platform shims: replace/encapsulate Windows/DirectX calls with SDL 1.2 equivalents for rendering, input, audio/music, and networking; preserve original behavior.
- Assets: expect game data under `CD/` and its existing subfolders; do not relocate or rename assets.
- Memory model: keep flat Win32 assumptions; avoid 16-bit/segmented constructs.
- Change scope: prefer minimal, localized shims over broad refactors; keep data layouts intact.
- Compatibility scaffolding: early ports can lean on `src/legacy_compat.h`/`legacy_compat.cpp` for placeholder enums, RTTI, globals, and helpers (e.g., `_makepath`, `stricmp`, stub `MixFileClass::Retrieve`, theater data, basic TechnoTypeClass/House/Building scaffolds, audio placeholders like `VolType`/`VoxType`, `SpeakQueue`, and `*_COUNT` enum sentinels). Replace these placeholders with real implementations as the surrounding systems get ported.
- If a port needs shared legacy enums/structs (e.g., `LayerType`, `MarkType`, new `RTTI_*` values) and the original header isn't ported yet, add a minimal equivalent to `src/legacy_compat.h` and keep the usage localized.
- When porting a legacy class that previously lived as a placeholder in `src/legacy_compat.h`, move it into a dedicated `src/<name>.h`/`src/<name>.cpp` pair and remove the placeholder to avoid diverging definitions.
- Loading shim: `FileClass`/`RawFileClass` in `legacy_compat` open from `CD/` by default and `Load_Alloc_Data` mirrors the original helper—reuse these when porting animation/type data that pulls raw assets.
- C++98 quirks: avoid defaulted special members and enum post-increment; iterate enums via int index and cast back. Prefer `snprintf` over `sprintf` to satisfy modern toolchains.
- Tracking: update `PROGRESS.md` when a file builds and runs correctly through the SDL/CMake path.
- Keep this document updated with new porting conventions or findings as they emerge.
