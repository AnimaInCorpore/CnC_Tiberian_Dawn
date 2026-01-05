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
- Validate changes with builds, tests, and comparisons to original behavior. Use tools like `runTests` for automated checks where possible.

## Mechanical Rules

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
