# Next steps
Tackle one chunk at a time; when a chunk has no remaining work, mark it with “Implementation done!”. Completed work is tracked in `PROGRESS.md`.

## Milestone: playable single-player loop (manual)
Status: Next steps. Scope: prove the current SDL/CMake build can boot, start a mission, reach a win/lose, return to the menu, and exit cleanly using only original assets from the repo-local `CD/...` mirrors.
- Exercise boot → title/menu → new game → in-mission → win/lose → back to menu → quit, and log any divergences in `PROGRESS.md`.
- Use the debug hook (`TD_AUTOSTART_SCENARIO`, plus `TD_AUTOSTART_LOAD_ONLY` / `TD_AUTOSTART_LOAD_TITLE` / `TD_AUTOSTART_DRAW_ONCE` / `TD_AUTOSTART_FRAMES`) to reproduce scenario-load issues without driving the UI (`src/maingame.cpp`).
- Re-verify palette fades against Win95, including `Fade_Palette_To(..., ..., NULL)` call sites (Win95 still shows the fade via hardware palette updates) (`src/interpal.cpp`, `src/wwlib_runtime.cpp`).
- Re-verify title/menu text rendering parity (font selection, gradient ramp selection via `TPF_USE_GRAD_PAL`, clipping/spacing) (`src/text.cpp`, `src/load_title.cpp`).

## Audio + music parity
Status: Next steps. Scope: match Win95 mixer behavior and theme playback so the port “feels” like C&C.
- Improve SDL mixer parity (priority/channel reservation, pan law, fade/stop semantics) and ensure menu sliders map to real mixer volume behavior (`src/audio_play.cpp`, `src/platform_audio_sdl.cpp`, `src/options.cpp`).
- Implement/verify theme/music streaming so `ThemeClass` matches Win95 selection/loop rules (`src/theme.cpp`).

## Movies: VQA audio + subtitle/EVA timing
Status: Next steps. Scope: VQAs should match Win95 A/V sync, subtitle timing, and skip rules.
- Add VQA audio playback routed through the mixer and verify subtitle/EVA timing against Win95 (`src/movie.cpp`, `src/vqa_decoder.cpp`, `src/audio_play.cpp`).

## Save/load + config persistence
Status: Next steps. Scope: allow normal sessions to persist/resume without the Win95 `SETUP.EXE`.
- Verify the full save/load UI flow parity (save/load/delete behaviors, description rules, error handling) (`src/loaddlg.cpp`, `src/saveload.cpp`).
- Verify profile/config writes hit disk and surface errors correctly (`RawFileClass::Error()` parity) (`src/rawfile.cpp`, `src/profile.cpp`, `src/port_paths.cpp`).
- Replace the minimal `CONQUER.INI` auto-creation with a behavior-equivalent setup/config flow while keeping the repo-local `CD/...` mirror working (`src/port_setup.cpp`, `src/options.cpp`).

## Deferred (after single-player is solid)
Status: Later. Scope: avoid expanding these until the above milestone is consistently reproducible.
- Multiplayer TCP/IP/session/lobby parity while keeping the UDP-backed IPX95 path (`src/tcpip.cpp`, `src/connect.cpp`, `src/ipx95.cpp`).
- Map editor entry points and `GAME_MAP_EDIT` loop parity (`src/maingame.cpp`, `src/mapedit.cpp`, `src/mapedsel.cpp`).

## Docs and hygiene
Status: Ongoing. Scope: keep contributor-facing docs aligned with the actual port state.
- Keep `PROGRESS.md`, `README.md`, and this file in sync as subsystems move from “ported” to “verified” (`PROGRESS.md`, `README.md`, `NEXT_STEPS.md`).
