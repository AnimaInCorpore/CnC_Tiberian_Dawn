# Porting rules

## Goals and constraints
- **Target**: CMake + g++ in C++98 mode (`-std=gnu++98`), producing `cnc_td`.
- **Source layout**: Ported sources go to `src/` with lowercase legacy filenames.
- **Assets**: Load directly from `CD/` (maintain original structure).
- **Platform**: Replace Win32/DirectX with SDL 1.2 (rendering, input, audio, network).
- **Memory**: Keep Win32 flat model; avoid 16-bit/segmented code.
- **Scope**: Thin compatibility layers over refactors; verify legacy behavior.

## Stub lifecycle
*Guidance on creating, placing, and removing temporary implementations.*

### 1. Creation & Placement
- **Missing/Broken Source**: If a file is missing or won't compile, add a stub `.cpp` in `src/` which includes `src/legacy_compat.h`. Ensure the CMake build remains stable.
- **Deferred I/O Modules**: If headers reference save/load methods as "implemented in ioobj.cpp", add `src/ioobj.cpp` stubs for those methods until `saveload.cpp` and pointer coding tables are ported.
- **Type Completeness**: Use `src/legacy_compat.h` for shared typedefs, enums, globals, and RTTI values.
- **Stub Headers**: If a header is needed for inheritance but unported, create a minimal dedicated stub header (e.g., `src/house.h`) rather than cluttering `legacy_compat.h`.
- **Large Subsystems**: For complex missing dependencies (e.g., networking, `foot.cpp` requiring `FootClass`), keep a stub `.cpp` and a minimal shared header to avoid premature compatibility layer bloat.

### 2. Implementation Strategy
- **Partial implementations**: Implement minimal `Process()` or no-op returns for UI dialogs/widgets dependent on unported state.
- **Feature Gates**: Provide no-op stubs for symbols guarded by flags like `CHEAT_KEYS` so default builds link.
- **Globals**: If a stub global becomes shared state, move its definition to `src/globals.cpp` and keep an `extern` declaration in `src/legacy_compat.h`.
- **Shim Extraction**: When a compatibility shim grows beyond a few helpers (e.g., a whole class), move it into `src/<module>.h/.cpp` and include it from `src/legacy_compat.h` to avoid header bloat and ODR risks.

### 3. Replacement (De-stubbing)
- **Migration**: When a real implementation lands, move code from `src/legacy_compat.*` to its dedicated `src/<name>.cpp/.h`.
- **Cleanup**: Remove the placeholder from the shim layer.
- **Include Updates**: Update `src/legacy_compat.h` to include the new header (reduce churn) or update call sites to include the new header explicitly (preferred for clarity).

## Include & Header Strategy
- **Wrapper Headers**: When a module includes `function.h`, provide a `src/function.h` wrapper that pulls in `src/legacy_compat.h` and standard headers.
- **Match Legacy Boundaries**: Keep legacy modules as separate `src/<name>.h/.cpp` pairs (e.g., `file.*` vs `rawfile.*`) to avoid “mega-shim” files and make PROGRESS tracking accurate.
- **Forward Declarations**: Prefer forward declarations over includes when defining shim types to avoid circular dependencies.
- **Covariant Returns**: Keep `TechnoTypeClass::Create_One_Of(...)` returning `ObjectClass*` in shims so derived overrides remain covariant.
- **Overlay Modules**: Legacy overlay `.cpp` files (e.g., `iomap.cpp`) may define methods now living in other ported modules; avoid duplicate definitions and keep the overlay file as a thin dispatcher into the new implementation where possible.

## Data Types, Memory & Layout
- **Legacy Widths**:
    - `COORDINATE` must remain wide (legacy `unsigned long`).
    - `CELL` must be `signed short`.
    - `TARGET` must be `unsigned short`.
    - *Rationale*: Preserves legacy overload resolution (e.g., `Sound_Effect`) and event/network structure layouts.
- **Unions & POD**: Structs inside unions (e.g., `EventClass::Data`) must remain POD (no user constructors/destructors).
- **Aggregate Initialization**: Keep shim structs for global tables as simple aggregates.
- **Safe Math**: Clamp shifts derived from byte tables. Use `const_cast` cautiously for legacy mutation of const assets (e.g. `bullet.ImageData`).

## C++98 Compatibility Checklist
- [ ] Build with `-std=gnu++98`.
- [ ] Use `NULL` instead of `nullptr`.
- [ ] Use `<stdint.h>` instead of `<cstdint>`.
- [ ] No defaulted functions (`= default`).
- [ ] Enums: Do not forward declare. Definition must precede usage.
- [ ] Legacy functions: Ensure `int` functions explicitly `return` a value (Watcom implicit return is UB).
- [ ] Strings: Use `snprintf` (or `::snprintf` fallback). Use `const char*` for literals.
- [ ] Static members: Define outside class without `static` keyword (e.g., `Type Class::Member;`).
- [ ] Iteration: Cast enum indices to `int` for loops.

## Stubs & Inheritance
- **Abstract Classes**: Ensure stub classes implement pure virtual methods from base classes (usually with empty bodies) to avoid "abstract class" instantiation errors.
- **Hierarchy**: Reconstruct required inheritance chains via dedicated stub headers (e.g., `sidebar.h` -> `power.h` -> `radar.h`).

## UI & Input Subsystems

### Graphics & Rendering
- **Shims**: `Draw_Box` and `Window_Box` implemented in `src/dialog.cpp`.
- **Caption/Textures**: Implement `Draw_Caption` and provide a usable `CC_Texture_Fill` fallback (solid/dither) so dialog UIs are readable before full SHP blitting lands.
- **Buffers**: `SeenBuff` is a 640x400 8-bit `GraphicBufferClass` backed by a software pixel buffer.
- **Present**: `Call_Back()`/`Main_Loop()` copy `SeenBuff` into the SDL 1.2 screen surface (8-bpp) and flip.
- **Redraw Flags**: Use `BooleanVectorClass` shim for `DisplayClass` bit arrays.
- **SDL Headers**: Prefer `#include <SDL.h>` (works with Homebrew/macOS and most SDL 1.2 installs); avoid hard-coding `SDL/SDL.h`.
- **SDL Linking**: Use `find_package(SDL)` when available; fall back to `pkg-config` (`sdl`) in CMake.

### Input & Geometry
- **Mouse**: `Get_Mouse_X/Y` returns SDL mouse coordinates from `SDL_Platform_Pump_Events`.
- **Keys**: Minimal key queue is provided by `SDL_Platform_Pop_Key()` (Esc/Return/Left/Right only, expand as needed).
- **Facing**: Implement `Desired_Facing8` early (8-way from screen-space vector) since UI widgets (e.g., dials) depend on it.
- **Calculations**: `Dir_Facing`, `Facing_Dir`, `Facing_To_32` are fully implemented bit/lookup conversions.
- **Keyboard**: Keep `KeyASCIIType` and `WWKEY` flags available.

### Gadgets & Controls
- **Layout**: Legacy code mutates fields (`X`, `Y`, `Width`) directly. Mirror this access.
- **Chaining**: Use `head.Add_Tail(child)` semantics.
- **Lifetime**: Many gadgets are stack-allocated. Avoid `delete` in list helpers.
- **Focus**: Maintain `Set_Focus`/`Has_Focus` shims.

## Filesystem & Assets
- **Paths**: `CDFileClass` handles DOS paths (`\`, `;`). Roots default to `CD/TIBERIAN_DAWN/`.
- **Loading**: Use `FileClass`/`RawFileClass` shims.
- **INI**: Minimal in-memory reader (`WWGetPrivateProfile...`). Writes are no-op.
- **Icons**: Stub `Get_Icon_Set_Map` to return NULL safely.
- **MIX archives**: `MixFileClass::Retrieve`/`Offset` are implemented as a portable loader in `src/mixfile.cpp` that lazily scans `CD/` for `.MIX` files and indexes them (no init-time `new MixFileClass(...)` required yet).
- **CRC convention**: MIX lookup uses the legacy rotate/add 32-bit CRC used elsewhere in the codebase (see `Add_CRC` / `NullModemConnClass::Compute_CRC` style), computed over the uppercased filename.

## Legacy Compatibility Shims
- **Macros**: Define `MAX(a,b)` and `MIN(a,b)` in `src/legacy_compat.h`.
- **Time**: Define `TICKS_PER_MINUTE` via `TICKS_PER_SECOND`.
- **Watcom/DOS**:
    - `DOSSegmentClass` is a heap-backed buffer.
    - `output()` is a no-op.
    - `Cardinal_To_Fixed` preserves legacy rounding/saturation logic.
