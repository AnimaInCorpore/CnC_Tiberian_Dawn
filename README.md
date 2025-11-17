
# Command & Conquer (Tiberian Dawn)

This repo tracks the ongoing modern port of the released Command & Conquer (Tiberian Dawn) source drop. The goal is to build the canonical 32-bit Windows code path with contemporary tooling while keeping the original Watcom-era sources available as reference.

## Project Scope
- Port legacy `.CPP`/`.H` files into a `src/` hierarchy that compiles with `g++`/Clang using `-std=gnu++14`.
- Retire DOS/16-bit branches and focus every translation unit on the Win32 behavior so future SDL/Emscripten backends match the original game.
- Replace missing third-party SDKs (DirectX 5, Greenleaf GCL, HMI SOS) with portable shims; until then, dependent subsystems remain stubbed.
- Track porting progress and header modernization in `PROGRESS.md` and `AGENTS.md`.

## Dependencies
| Purpose | Requirements |
| --- | --- |
| **Modern port (experimental)** | CMake ≥ 3.20, a C++14 compiler with GNU extensions enabled (tested with `g++` 6+/Clang 12+), and a build backend such as Ninja or Make. SDL and other runtime replacements are still TODO. |
| **Legacy build artefacts** | Original third-party SDKs plus the Watcom C/C++ 10.6 compiler, Borland TASM 4.0, and MASM 6.11d. The proprietary “core engine” libraries are *not* included; see the Red Alert release for reference material. |

## Building
### Modern CMake target (recommended for contributors)
From the repository root:

```bash
cmake -S . -B build -G Ninja   # drop -G Ninja to use your default generator
cmake --build build
```

This produces the static library `build/libtd_port.a` (or `td_port.lib` on MSVC) from the files already migrated into `src/`. The build also writes `build/compile_commands.json` for editors or static analysis tools.

### Legacy Watcom/Borland pipeline (reference only)
Recreate the original toolchain, obtain DirectX 5 SDK, GCL, and HMI SOS, then wire the shipped make/project files (`BFILE.MAK`, `CONQUER.LNT`, etc.) back together. This path is currently incomplete: missing libraries and platform-specific assembly sources must be restored manually.

## Documentation
- `AGENTS.md` & `PROGRESS.md` describe the porting guidelines and per-file migration status.
- `LICENSE.md` contains the GPLv3-based license and additional terms from EA.

## Support & Ownership
This repository is provided for preservation and will not accept upstream support or contributions. You must own Command & Conquer to use any resulting binaries (EA App, Steam, etc.).
