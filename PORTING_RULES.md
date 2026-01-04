# Porting Rules (C++ Baseline)

This document defines the **mechanical, source-level rules** used while porting the original C&C95 code into this repo. It exists to keep diffs consistent, portable, and compatible with the project’s chosen C++ standard while preserving original behavior.

This document complements `AGENTS.md` (which describes the overall porting workflow and goals).

## Maintenance

- Apply these rules for every ported change.
- If you discover a new *project-wide* mechanical rule (i.e., it should be applied consistently across many files), add it here before (or along with) applying it elsewhere.

## Baseline

- **Language standard:** ISO **C++17** (see `CMakeLists.txt`). Do not introduce C++20+ features.
- **Portability first:** avoid platform-specific code outside the platform/SDL layers.
- **Behavior first:** do not “modernize” logic; prefer minimal diffs that compile cleanly.

## Mechanical Rules

### Null pointers

- Use `nullptr` for null pointer constants (instead of `NULL` or integer `0`).
  - Prefer `return nullptr;` over `return NULL;` / `return(0);` when returning pointers.
- Do not pass a bare `nullptr` through **varargs** (e.g., `printf`, `sprintf`, custom `...` APIs). Cast to the expected pointer type:
  - Example: `static_cast<const char*>(nullptr)` / `static_cast<void*>(nullptr)` (as appropriate).

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

### Headers and standard functions

- Prefer C++ headers (`<cstdint>`, `<cstring>`, `<cstdio>`, `<cstdlib>`, …) over C headers.
- Ensure the correct header is included instead of relying on incidental transitive includes.
- Prefer existing compatibility helpers (e.g., `src/include/legacy/compat.h`, `src/include/legacy/windows_compat.h`) over introducing new platform `#ifdef`s in gameplay/UI code.

### Struct layout and binary compatibility

- Preserve struct layout when data is persisted, serialized, or shared across modules.
- If you must change packing/alignment-related code, add a `static_assert(sizeof(Type) == expected)` where practical.

### Ownership and allocation

- Keep existing ownership/allocation patterns unless a change is required to compile or to fix a proven bug.
- Avoid introducing new lifetime semantics (e.g., widespread `std::unique_ptr` refactors) as part of mechanical porting work.
