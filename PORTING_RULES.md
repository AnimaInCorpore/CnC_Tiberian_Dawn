# Porting rules

## Goals and constraints
- **Target**: CMake + g++ in C++98 mode (`-std=gnu++98`), producing `cnc_td`.
- **Source layout**: Ported sources go to `src/` with lowercase legacy filenames.
- **Progress**: Keep `PROGRESS.md` in sync with what exists in `src/` (annotate `(stub)`/`(shim)` so checkmarks remain meaningful).
- **Assets**: Load directly from `CD/` (maintain original structure).
- **Platform**: Replace Win32/DirectX with SDL 1.2 (rendering, input, audio, network).
- **Memory**: Keep Win32 flat model; avoid 16-bit/segmented code.
- **Scope**: Thin compatibility layers over refactors; verify legacy behavior.

## Stub lifecycle
*Guidance on creating, placing, and removing temporary implementations.*

### 1. Creation & Placement
- **Missing/Broken Source**: If a file is missing or won't compile, add a stub `.cpp` in `src/` that includes `function.h` (preferred umbrella) or `legacy_compat.h`. Ensure the CMake build remains stable.
- **Deferred I/O Modules**: If headers reference save/load methods as "implemented in ioobj.cpp", add `src/ioobj.cpp` stubs for those methods until `saveload.cpp` and pointer coding tables are ported.
- **Type Completeness**: Keep shared typedefs/enums/RTTI/globals in `src/legacy_compat.h` early; for larger shared structs prefer a focused shim header (e.g., `src/path.h`) and only forward-declare from `legacy_compat.h`.
- **Stub Headers**: If a header is needed for inheritance but unported, create a minimal dedicated stub header (e.g., `src/house.h`) rather than cluttering `legacy_compat.h`.
- **Large Subsystems**: For complex missing dependencies (e.g., networking, `foot.cpp` requiring `FootClass`), keep a stub `.cpp` and a minimal shared header to avoid premature compatibility layer bloat.

### 2. Implementation Strategy
- **Partial implementations**: Implement minimal `Process()` or no-op returns for UI dialogs/widgets dependent on unported state.
- **Feature Gates**: Provide no-op stubs for symbols guarded by flags like `CHEAT_KEYS` so default builds link.
- **Abstract Classes**: Ensure stub classes implement pure virtual methods from base classes (usually with empty bodies) to avoid "abstract class" instantiation errors.
- **Hierarchy**: Reconstruct required inheritance chains via dedicated stub headers (e.g., `sidebar.h` -> `power.h` -> `radar.h`).
- **Globals**: If a stub global becomes shared state, move its definition to `src/globals.cpp` and declare it in the owning header (or temporarily in `src/legacy_compat.h` while call sites are still fluid).
- **Shim Extraction**: When a compatibility shim grows beyond a few helpers (e.g., a whole class), move it into `src/<module>.h/.cpp` to avoid ODR risks and keep `legacy_compat.*` small.

### 3. Replacement (De-stubbing)
- **Migration**: When a real implementation lands, move code out of `src/legacy_compat.*` into its dedicated `src/<name>.cpp/.h`.
- **Cleanup**: Remove the placeholder from the shim layer.
- **Include Updates**: During churn-heavy phases, `src/legacy_compat.h` may include the new header; once stable, prefer updating call sites to include the owning header explicitly.

## Include & Header Strategy
- **Wrapper Headers**: Keep `src/function.h` as the umbrella include for legacy modules (it pulls in `legacy_compat.h` and small standard headers).
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
- **Event `ID` bitfield**: When legacy code assigns `ID = Houses.ID(PlayerPtr);`, use `ID = (PlayerPtr ? (unsigned)PlayerPtr->Class->House : 0u) & 0x0Fu;` to keep values within the 4-bit `EventClass::ID` field and avoid depending on the unported `Houses` manager.
- **`DirType` values**: Keep direction constants consistent with legacy `DEFINES.H` (8-way is `(n<<5)` on a 0–255 circle; include special `DIR_SW_X1`/`DIR_SW_X2` used by harvester/refinery tracks).
- **Unions & POD**: Structs inside unions (e.g., `EventClass::Data`) must remain POD (no user constructors/destructors).
- **Aggregate Initialization**: Keep shim structs for global tables as simple aggregates.
- **Safe Math**: Clamp shifts derived from byte tables. Use `const_cast` cautiously for legacy mutation of const assets (e.g. `bullet.ImageData`).

## C++98 Compatibility Notes
- **Keywords/headers**: Use `NULL` (not `nullptr`), prefer `<stdint.h>` (not `<cstdint>`), and avoid C++11+ features like `= default`.
- **Enums**: Do not forward declare. Definition must precede usage.
- **Returns**: Ensure non-void functions explicitly `return` a value (Watcom implicit returns are UB).
- **Strings**: Use `::snprintf` (avoid `std::snprintf`, not guaranteed in C++98).
- **Static members**: Define outside the class without repeating `static` (e.g., `Type Class::Member;`).
- **Iteration**: Cast enum indices to `int` in loops when needed.
- **Watcom pragmas**: Remove or `#ifdef __WATCOMC__`-guard `#pragma warn ...` and similar compiler-specific directives.

## UI & Input Subsystems

### Graphics & Rendering
- **Shims**: `Draw_Box` and `Window_Box` implemented in `src/dialog.cpp`.
- **Caption/Textures**: Implement `Draw_Caption` and provide a usable `CC_Texture_Fill` fallback (solid/dither) so dialog UIs are readable before full SHP blitting lands.
- **Pages/Viewports**: Implement `GraphicBufferClass` + `GraphicViewPortClass` in `src/wwgfx.h` and `src/wwgfx.cpp` as 8-bit software surfaces (emulating the C&C95 DirectDraw-era API).
- **Buffers**: Use `HiddenPage` (back buffer) + `VisiblePage` (front buffer) with `HidPage`/`SeenBuff` viewports attached at 640x400.
- **Rect Semantics**: `Fill_Rect`/`Draw_Rect` take inclusive coordinates `(x1, y1, x2, y2)` (legacy call sites often pass `x+w-1`, `y+h-1`).
- **Present**: `Call_Back()`/`Main_Loop()` should pump events, blit `HidPage` -> `SeenBuff`, then `SDL_Platform_Present_Indexed8(SeenBuff.Data(), ...)` (do not clear pages during present).
- **Redraw Flags**: Use `BooleanVectorClass` shim for `DisplayClass` bit arrays.
- **Palette Tables**: Until palette builders (`Build_Translucent_Table`, fading table generation) are ported, initialize `DisplayClass` tables (`Fading*`, `RemapTables`, `TranslucentTable`, `ShadowTrans`, etc.) to identity mappings in `DisplayClass::One_Time()` to avoid uninitialized reads.
- **SDL Headers**: Prefer `#include <SDL.h>` (works with Homebrew/macOS and most SDL 1.2 installs); avoid hard-coding `SDL/SDL.h`.
- **SDL Linking**: Use `find_package(SDL)` when available; fall back to `pkg-config` (`sdl`) in CMake.

### Input & Geometry
- **Mouse**: `SDL_Platform_Pump_Events` updates cached mouse state; `Get_Mouse_X/Y` reads it.
- **Keys**: Minimal key queue is provided by `SDL_Platform_Pop_Key()` (Esc/Return/Up/Down/Left/Right only, expand as needed).
- **Facing**: Implement `Desired_Facing8` early (8-way from screen-space vector) since UI widgets (e.g., dials) depend on it.
- **Calculations**: `Dir_Facing`, `Facing_Dir`, `Facing_To_32` are fully implemented bit/lookup conversions.
- **Keyboard**: Keep `KeyASCIIType` and `WWKEY` flags available.

### Gadgets & Controls
- **Layout**: Legacy code mutates fields (`X`, `Y`, `Width`) directly. Mirror this access.
- **Chaining**: Use `head.Add_Tail(child)` semantics.
- **Buttons**: `TextButtonClass` supports chained `Draw_All()` and hit-testing via `Input()` when called on the list head.
- **Lifetime**: Many gadgets are stack-allocated. Avoid `delete` in list helpers.
- **Focus**: Maintain `Set_Focus`/`Has_Focus` shims.

## Filesystem & Assets
- **Paths**: `CDFileClass` handles DOS paths (`\`, `;`). Default search roots check `CD/TIBERIAN_DAWN/CD{2,1,3}/`, then `CD/TIBERIAN_DAWN/`, then `CD/`.
- **Loading**: Use `FileClass`/`RawFileClass` shims.
- **INI**: Minimal in-memory reader (`WWGetPrivateProfile...`). Writes are no-op.
- **Icons**: Stub `Get_Icon_Set_Map` to return NULL safely.
- **MIX archives**: `MixFileClass::Retrieve`/`Offset` are implemented as a portable loader in `src/mixfile.cpp` that lazily scans `CD/` for `.MIX` files and indexes them (no init-time `new MixFileClass(...)` required yet).
    - Some shipped `.MIX` files (e.g. `CCLOCAL.MIX`, `UPDATE.MIX`, `UPDATA.MIX` in this CD tree) include an embedded XCC name table entry (`"XCC by Olaf van der Spek"`), which can be used for direct name lookups without relying on the legacy filename CRC algorithm.
    - For mixes without an embedded name table, filename-CRC lookup may still be needed; keep the CRC implementation isolated to `src/mixfile.cpp` so it can be corrected/validated without touching call sites.

## Legacy Compatibility Shims
- **Macros**: Define `MAX(a,b)` and `MIN(a,b)` in `src/legacy_compat.h`.
- **Time**: Define `TICKS_PER_MINUTE` via `TICKS_PER_SECOND`.
- **Watcom/DOS**:
    - `DOSSegmentClass` is a heap-backed buffer.
    - `output()` is a no-op.
    - `Cardinal_To_Fixed` preserves legacy rounding/saturation logic.
