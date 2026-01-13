# Porting rules

## Goals and constraints

- Target build: CMake + g++ in C++98 mode (`-std=gnu++98` if needed) producing the `cnc_td` binary.
- Source layout: place ported sources in `src/` using the legacy makefile’s lowercase filenames (e.g., `conquer.cpp`).
- Assets: load directly from `CD/` and its existing subfolders; do not relocate or rename assets.
- Platform/behavior: replace/encapsulate Win32/DirectX with SDL 1.2 equivalents (rendering, input, audio/music, networking) while preserving legacy behavior.
- Memory model: keep flat Win32 assumptions; avoid 16-bit/segmented constructs.
- Change scope: prefer thin compatibility layers and localized fixes over broad refactors; keep data layouts intact.

## Compatibility layer strategy (`src/legacy_compat.*`)

- Keep `src/legacy_compat.h`/`src/legacy_compat.cpp` as the staging area for shared typedefs/enums, RTTI values, globals, and small helpers needed by early ports (e.g., `_makepath`, `stricmp`, theater data, placeholder `MixFileClass::Retrieve`, basic `TechnoTypeClass`/`House`/`Building` scaffolds, `VOL_*` constants, `SpeakQueue`, `*_COUNT` sentinels).
- Legacy macro expectations: define both `MAX(a,b)` and `MIN(a,b)` in `src/legacy_compat.h` (many modules assume both exist).
- Type creation seam: keep `TechnoTypeClass::Create_One_Of(...)` returning `ObjectClass*` in the shim layer so derived `*TypeClass` overrides that return `ObjectClass*` remain covariant in C++98.
- Timer constants: when a port references `TICKS_PER_MINUTE`, define it in `src/legacy_compat.h` in terms of `TICKS_PER_SECOND` (avoid sprinkling per-file copies).
- When a legacy module includes `function.h`, keep the include and provide a thin `src/function.h` wrapper that pulls in `src/legacy_compat.h` (and any standard headers needed) rather than rewriting include lists.
- If a port needs shared legacy enums/structs (e.g., `LayerType`, `MarkType`, new `RTTI_*` values) and the original header isn’t ported yet, add a minimal equivalent to `src/legacy_compat.h` and keep usage localized.
- Header hygiene: when adding shim types that reference other shim types (e.g., `FileClass`), add forward declarations first to avoid include-order fragility.
- De-stubbing rule: when a real implementation lands for a class/function that previously existed as a placeholder in `src/legacy_compat.*` (or in catch-all headers like `src/gadget.h`), move it into dedicated `src/<name>.h`/`src/<name>.cpp`, remove the placeholder, and (if it reduces churn) have `src/legacy_compat.h` include the new header.
- Keep call sites honest: once a stub is removed from `src/legacy_compat.h`, include the new dedicated header explicitly where needed (e.g., `FuseClass` is now in `src/fuse.h`/`src/fuse.cpp`).
- Globals: when a stub global in `src/legacy_compat.cpp` becomes shared game state, move the definition into `src/globals.cpp` and keep only the `extern` declaration in headers (typically `src/legacy_compat.h`) to avoid duplicate definitions.

## Stubs, feature gates, and build stability

- If a `.cpp` is missing from the repo snapshot or can’t compile yet due to unported dependencies, add a stub translation unit under `src/` (include `src/legacy_compat.h` or the module’s new header) so CMake builds stay stable; annotate `PROGRESS.md` and replace the stub with the real code once dependencies land.
- If an unported header is needed only for type completeness (e.g., a base class for inheritance), prefer a minimal dedicated stub header under `src/` (same lowercase name) rather than inflating `src/legacy_compat.h`; replace it when the real header is ported.
- For UI dialogs that depend on still-unported option state or widget implementations (e.g., `OptionsClass`, `GaugeClass`/`SliderClass`), prefer a small stub `Process()` implementation over adding premature placeholders to the compatibility layer.
- For movement-heavy modules that depend on unported runtime state or debug drawing surfaces (e.g., `foot.cpp` / `findpath.cpp` needing `FootClass` internals, `Map` pixel conversion, or `SeenBuff`), keep a stub `.cpp` until the required subsystems are in place rather than expanding the compatibility layer with ad-hoc placeholders.
- For large modules that pull in still-missing subsystems (e.g., Win32 serial/modem + multiplayer globals in `nulldlg.cpp`), keep a stub `.cpp` in `src/` and introduce only the minimal shared header surface (e.g., `src/wincomm.h` with `SerialSettingsType` and related constants) to avoid exploding the compatibility layer prematurely.
- For the serial/modem connection manager (`nullmgr.cpp`), keep a dedicated stub (`src/nullmgr.h`/`src/nullmgr.cpp`) with a minimal `NullModemClass` API and a small `BuildBuf` backing store, since other legacy modules cast `NullModem.BuildBuf` to their packet types.
- If a legacy module is compiled only under a flag (e.g., `CHEAT_KEYS`) but its symbols are referenced elsewhere, provide no-op stubs so default builds link; replace with real behavior once dependencies land.
- For `CHEAT_KEYS`-only `Debug_Dump(MonoClass*)` helpers, forward-declare `MonoClass` in the header and keep the implementation safe to compile even if the real mono/debug screen class isn’t ported yet (e.g., `(void)mono;` no-op).
- As individual modules get ported, migrate module-specific stub logic out of `src/legacy_compat.cpp` into the corresponding `src/<module>.cpp` to keep the compatibility layer small and avoid accidental divergence.

## Types, headers, and legacy call sites

- Legacy typedefs: keep `COORDINATE` wide (legacy headers use `unsigned long`) so call sites can disambiguate overloads like `Sound_Effect(voc, volume)` vs `Sound_Effect(voc, coord)` without refactoring.
- Match legacy scalar widths for overload sets and packed data: keep `CELL` as `signed short` and `TARGET` as `unsigned short` (per `DEFINES.H`) so legacy overloads/constructors remain valid (e.g., `EventClass(EventType,int)` vs `EventClass(EventType,TARGET)`), and to preserve on-the-wire/event-queue layouts.
- Union/POD rule: structs stored inside a `union` (e.g., `SpecialClass` inside `EventClass::Data`) must remain POD in C++98 — avoid user-provided constructors/destructors; rely on zero-init for globals and explicit initialization when needed.
- When porting a header that included other unported headers, prefer forward declarations plus `src/legacy_compat.h` for shared typedefs/enums (e.g., `OverlayType`, `SmudgeType`) to keep include-order churn low.
- Object lists: legacy code frequently chains objects via `ObjectClass::Next` (and cargo/passenger holds via `FootClass`); prefer adding the minimal fields/methods to the shim layer rather than rewriting call sites.
- Map/cell shim: `Map` is a `MapClass` returning `CellClass&` so legacy `Map[cell]....` call sites compile; `Map.Flag_To_Redraw(...)` is stubbed; ensure `CellClass` is complete at call sites (include `src/cell.h` when needed).
- Mixed access patterns: some legacy code mixes `house->Class.House` and `house->Class->House`; prefer a small proxy wrapper in the shim layer so both compile without touching call sites.

## Filesystem and asset loading

- Loading helpers: reuse `FileClass`/`RawFileClass` and `Load_Alloc_Data` when porting animation/type data that pulls raw assets; these shims should open from `CD/` by default.
- Icon set helpers: early ports may rely on stub `Get_Icon_Set_Map`/`Register_Icon_Set`; keep these NULL-safe so `Occupy_List`/rendering helpers don’t crash before the real icon-set parser/cache is ported.
- File I/O shims: `CDFileClass` (`src/cdfile.cpp`) implements DOS-style semicolon search paths (including `?:\\`), normalizes `\\` to `/`, and seeds default roots under `CD/TIBERIAN_DAWN/`; `MixFileClass::Offset` is still a stub and `CCFileClass` falls back to direct disk access until mixfiles are ported.
- INI helpers: `WWGetPrivateProfileInt`/`WWGetPrivateProfileString` are implemented as a minimal in-memory INI reader in `src/legacy_compat.cpp`; `WWWritePrivateProfile*` are no-op “success” stubs and should be replaced when save/INI writing is ported.

## UI and widget shims

- UI draw/text: `Draw_Box`/`Window_Box`/`Fancy_Text_Print`/`Conquer_Clip_Text_Print` live in `src/dialog.cpp`, but `GraphicPageClass` work is still stubbed until the display layer is real; keep UI code NULL-safe when calling into graphics helpers.
- Default-arg compatibility: expose legacy-style defaults in declarations where practical (e.g., `Conquer_Clip_Text_Print(..., tabs=0)`) to avoid churn across call sites.
- Cell redraw flags: `DisplayClass` uses a bit array for per-cell redraw; use the `BooleanVectorClass` shim (`Resize`, `Is_True`, `operator[]` assignment) rather than rewriting call sites.
- Mouse/facing: UI widgets (e.g., the facing dial) expect `Get_Mouse_X/Y` and `Desired_Facing8`; keep these stubbed until the input/geometry stack is ported. `Dir_Facing`/`Facing_Dir`/`Facing_To_32` are pure bit/lookup conversions and can be implemented directly (see `src/legacy_compat.h`).
- UI focus/keyboard: keep `GadgetClass::{Set_Focus,Has_Focus,Clear_Focus}` and `KeyASCIIType`/`Keyboard::To_ASCII`/`WWKEY_*_BIT` flags available for legacy UI input filters (e.g., `edit.cpp`).
- UI sticky dragging: sliders/gauges check `GadgetClass::StuckOn` while processing `LEFTHELD`; keep `StuckOn` available to derived classes (protected) to avoid rewriting legacy `Action()` logic.
- UI gadget placeholders: until the full gadget/control stack is ported, lightweight placeholders may live in standalone headers like `src/list.h` and `src/gadget.h` (e.g., checklist/config UI); migrate to real `src/<name>.h`/`src/<name>.cpp` implementations as their legacy counterparts are ported.
- Gadget chaining/layout: legacy UI code mutates `GadgetClass` geometry fields directly (`X`, `Y`, `Width`, `Height`) and calls `Add_Tail` repeatedly on the same head; keep fields accessible and keep the portable build’s semantics as `head.Add_Tail(child)` (append-to-tail).
- Gadget list lifetime: many dialogs build gadget chains from stack objects; avoid `delete`-based list helpers in the portable `GadgetClass` until gadget allocation patterns are clarified (`GadgetClass::Delete_List` currently detaches only).
- WWLIB containers: provide a minimal `VectorClass<T>` shim in `src/legacy_compat.h` (std::vector-backed) so legacy classes can keep inheriting from it while the real container stack is unported.
- Graphics buffers: add no-op `Clear()` to the `GraphicPageClass` shim so ports can keep calling `page.Clear()` without pulling in the full renderer yet.

## Data/layout correctness and UB avoidance

- Palette morph table sizes: keep `MAGIC_COL_COUNT`/`SHADOW_COL_COUNT`/`USHADOW_COL_COUNT` defined (from legacy `DEFINES.H`) so static translucent/shadow tables keep stable sizes during the port.
- Null text vs. text IDs: legacy code sometimes passes `TXT_NONE` where a `char const*` “no text” pointer is expected; if `TXT_*` are enums in the shim layer, use an explicit null pointer (`(char const*)0`) at the call site.
- Data-table structs: if legacy code uses brace initializers for global tables (e.g., `Warheads`, `Weapons`), keep the corresponding shim structs as simple aggregates (no user-defined constructors) so C++98 aggregate initialization works.
- Shift safety: when shifting by a value read from legacy byte tables (e.g., warhead `SpreadFactor`), clamp the shift to a sane range to avoid UB on modern compilers.
- Const asset caches: when legacy code mutates cached asset pointers on a `const` reference (e.g., `bullet.ImageData`), prefer `const_cast<void const*&>(...)` over C-style casts.
- Mutable data tables: if legacy code “patches” a `static const` table during a one-time init (typically via a cast), make the table object non-const and keep external references `const*` to avoid UB.

## C++98 portability notes

- C++98 quirks: avoid defaulted special members and enum post-increment; iterate enums via an `int` index and cast back.
- Prefer bounded formatting: use `snprintf`/`vsnprintf` over `sprintf` when touching formatting code; don’t assume `std::snprintf` exists on every older libstdc++ (fall back to `::snprintf` if needed).
- Avoid C++11-only library dependencies in new code (headers like `<cstdint>`, language features like `nullptr`, etc.); prefer C89/C99 headers such as `<stdint.h>` or shim typedefs where needed.
- Use `const char*` for string literals, and don’t qualify static member declarations inside class bodies (write `static T Member;`, not `static T Class::Member;`).
- Enum declarations: avoid forward-declaring enums (some C++98 toolchains reject it); place function declarations after the enum definition or use integral types at the seam.

## Legacy Watcom/DOS pieces

- Watcom `#pragma aux` / inline asm: replace with portable helpers that emulate 16-bit register behavior (cast/mask through `int16_t`) and avoid implementation-defined signed right shifts by using explicit floor division when needed.
- DOS/DPMI helpers: `DOSSegmentClass`/`output()` are DOS-only; in the portable build keep the interface, implement `DOSSegmentClass` as a heap-backed byte buffer, and make `output()` a no-op.
- Fixed-point helpers: `Cardinal_To_Fixed`/`Fixed_To_Cardinal` are ported from `COORDA.ASM` as `src/coorda.cpp`; preserve legacy rounding (`+0x80` before `>>8`) and saturation to `0xFFFF`.

## Diagnostics and tracking

- Debug logging: stub `CCDebugString` prints to stderr until the original Win32 debug plumbing is ported.
- Encoding: some legacy sources may contain non-UTF-8 bytes; if tooling can’t patch/read the copied `src/*.cpp`, convert to UTF-8 (e.g., via `iconv`) or replace with a UTF-8 stub until the real port is ready.
- Tracking: update `PROGRESS.md` when a file builds and runs correctly through the CMake + SDL 1.2 path, and add new portability conventions here as they emerge (dedupe when possible).
