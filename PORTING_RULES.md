# Porting rules

## Project invariants (do not regress)
- **Build**: CMake + g++/clang++ in C++98 mode (`-std=gnu++98`), producing `cnc_td`.
- **Sources**: Ported code lives in `src/` using the same lowercase filenames as the legacy makefile.
- **Assets**: Load directly from `CD/` (do not rename or relocate anything under it).
- **Platform**: Replace Win32/DirectX services with SDL 1.2 equivalents (rendering, input, audio/music, networking).
- **Memory model**: Keep a flat Win32-era model; avoid segmented/DPMI-era behavior in new code.
- **Churn**: Prefer thin compatibility layers over refactors; preserve legacy logic/layouts first, then improve.
- **Tracking**: Keep `PROGRESS.md` consistent with what exists in `src/` (use `(stub)` / `(shim)` notes so `[x]` stays meaningful).

## Workflow: stubs → shims → real code
- **When a file is missing or won’t compile**: Add a stub `.cpp` (and if needed a minimal header) in `src/` so the build stays linkable.
- **Prefer dedicated headers over “mega-shims”**: Keep `src/legacy_compat.h` for truly shared primitives; extract growing shims into `src/<module>.h/.cpp`.
- **Abstract/base class safety**: Stub derived classes must implement any pure virtual methods to avoid “abstract class” instantiation errors.
- **Inheritance chains**: Recreate only what is required (e.g., `sidebar.h` → `power.h` → `radar.h`) and keep it minimal.
- **Globals**: If a stubbed global becomes shared state, define it once in `src/globals.cpp` and declare it in the owning header.
- **De-stubbing**: When a real implementation lands, move code out of `src/legacy_compat.*`, delete the placeholder, and update includes.

## Includes and module boundaries
- **Umbrella include**: Keep `src/function.h` as the “legacy-like” umbrella include (it pulls in `legacy_compat.h` plus small standard headers).
- **Keep legacy splits**: Port as separate `src/<name>.h/.cpp` pairs (e.g., `file.*` vs `rawfile.*`) so PROGRESS tracking stays accurate.
- **Forward declarations**: Prefer forward declarations to avoid circular includes when defining shim types.
- **Covariant returns**: Keep `TechnoTypeClass::Create_One_Of(...)` returning `ObjectClass*` in shims so derived overrides remain covariant.
- **Overlay `.cpp` files**: If an overlay (e.g., `iomap.cpp`) historically defined methods now living elsewhere, avoid duplicate definitions; keep it as a dispatcher where possible.

## C++98 portability checklist (Watcom → g++)
- **No C++11+**: `NULL` (not `nullptr`), no `= default`, no `<cstdint>`, no `std::snprintf`.
- **Enums**: Do not forward declare; definition must precede usage.
- **Returns**: Ensure non-void functions explicitly `return` a value (Watcom implicit returns are UB).
- **Static members**: Define outside the class without repeating `static` (e.g., `Type Class::Member;`).
- **Iteration**: Cast enum indices to `int` in loops when needed.
- **Out-of-class defs**: Remove stray `virtual` on method definitions (Watcom accepted it; g++ rejects it).
- **Compiler pragmas**: Remove or guard Watcom-specific pragmas (e.g., `#ifdef __WATCOMC__`).

## Data, layout, and “don’t break networking/saves” rules
- **Legacy widths must stay**:
  - `COORDINATE` is `unsigned long`
  - `CELL` is `signed short`
  - `TARGET` is `unsigned short`
- **Event `ID` bitfield**: When legacy code uses `ID = Houses.ID(PlayerPtr);`, use `ID = (PlayerPtr ? (unsigned)PlayerPtr->Class->House : 0u) & 0x0Fu;` so it stays within the 4-bit `EventClass::ID` field.
- **`DirType` values**: Keep direction constants consistent with legacy `DEFINES.H` (8-way is `(n<<5)` on a 0–255 circle; keep `DIR_SW_X1`/`DIR_SW_X2` for harvester/refinery tracks).
- **Unions & POD**: Types stored inside unions (e.g., `EventClass::Data`) must remain POD (no user constructors/destructors).
- **Global tables**: Keep shim structs for global tables as simple aggregates.
- **Math safety**: Clamp shifts derived from byte tables; use `const_cast` sparingly for legacy “const but mutated” assets (e.g. `bullet.ImageData`).

## SDL 1.2 graphics/input conventions (current portable stack)
- **Software surfaces**: `GraphicBufferClass` + `GraphicViewPortClass` live in `src/wwgfx.h` + `src/wwgfx.cpp` as 8-bit buffers emulating the DirectDraw-era API.
- **Buffers**: `HiddenPage` (back) + `VisiblePage` (front) with `HidPage`/`SeenBuff` viewports at 640×400.
- **Rect semantics**: `Fill_Rect`/`Draw_Rect` use inclusive coordinates `(x1, y1, x2, y2)`; many legacy call sites pass `x+w-1`, `y+h-1` (or decrement `w/h` first).
- **Present path**: `Call_Back()` / `Main_Loop()` pump events, `HidPage.Blit(SeenBuff)`, then `SDL_Platform_Present_Indexed8(SeenBuff.Data(), ...)` (do not clear pages during present).
- **Dialog/UI drawing**: `Draw_Box` / `Window_Box` are in `src/dialog.cpp`; `Draw_Caption` / `CC_Texture_Fill` fallback live in `src/legacy_compat.cpp` until SHP blitting is fully ported.
- **Palette tables**: Until palette builders are ported, initialize `DisplayClass` tables (`Fading*`, `RemapTables`, `TranslucentTable`, `ShadowTrans`, etc.) to identity mappings in `DisplayClass::One_Time()` to avoid uninitialized reads.
- **Redraw flags**: Use the `BooleanVectorClass` shim for `DisplayClass` bit arrays.
- **SDL headers**: Prefer `#include <SDL.h>`; avoid hard-coding `SDL/SDL.h`.
- **SDL linking**: Use `find_package(SDL)` when available; fall back to `pkg-config` (`sdl`) in CMake.
- **Input**: `SDL_Platform_Pump_Events` maintains cached mouse position + left/right pressed/released/down (and click coordinates); `SDL_Platform_Pop_Key()` queues Esc/Return/Backspace/arrows + basic ASCII (key repeat enabled).
- **Geometry helpers**: Implement `Desired_Facing8` early; keep `Dir_Facing`, `Facing_Dir`, `Facing_To_32` conversions correct.

## UI controls (gadget stack expectations)
- **Direct field mutation**: Legacy code writes `X`, `Y`, `Width`, etc. directly; preserve this layout/access.
- **List semantics**: Use `head.Add_Tail(child)` and allow `Draw_All()` / `Input()` on the list head.
- **Lifetime**: Many gadgets are stack-allocated; avoid `delete` in list helpers.
- **Focus**: Maintain `Set_Focus` / `Has_Focus` behavior.
- **Gadget input**: `GadgetClass::Input()` now dispatches via `SDL_Platform_*`; ensure `SDL_Platform_Pump_Events` runs before gadget input each tick.
- **List entries**: `ListClass` stores heap-copied, writable strings; `Remove_Item` matches by string contents (not pointer identity).

## Filesystem and asset loading
- **DOS-ish paths**: `CDFileClass` handles `\\` and `;` lists. Default search roots probe `CD/TIBERIAN_DAWN/CD{2,1,3}/`, then `CD/TIBERIAN_DAWN/`, then `CD/`.
- **File I/O shims**: Use `FileClass` / `RawFileClass` wrappers for portable access.
- **INI**: `WWGetPrivateProfile...` reads from an in-memory buffer; `WWWritePrivateProfile...` is a no-op (returns success).
- **Icons**: `Get_Icon_Set_Map` safely returns `NULL`.
- **MIX**: `MixFileClass::Retrieve` / `Offset` are implemented in `src/mixfile.cpp` by lazily scanning `CD/` for `.MIX` and indexing entries.
  - Some shipped mixes embed an XCC name table entry (`"XCC by Olaf van der Spek"`) enabling direct name lookup.
  - For mixes without a name table, filename-CRC lookup may be needed; keep CRC logic isolated to `src/mixfile.cpp`.

## “Legacy environment” shims (keep centralized, then extract)
- **Macros**: `MAX(a,b)` / `MIN(a,b)` live in `src/legacy_compat.h`.
- **Time**: `TICKS_PER_MINUTE` derives from `TICKS_PER_SECOND`.
- **Watcom/DOS**: `DOSSegmentClass` is heap-backed; `output()` is a no-op; `Cardinal_To_Fixed` preserves legacy rounding/saturation.
