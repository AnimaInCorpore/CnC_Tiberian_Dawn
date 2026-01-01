# Next steps to port the project
Tackle one chunk at a time; when a chunk has no remaining next steps, mark it with "Implementation done!". Completed work is tracked in `PROGRESS.md`.

## What “playable” means (single-player)
Status: Definition. Scope: the SDL build should let a user boot to the main menu, start a mission, play it to a win/lose, and exit cleanly using only original game assets (no built-in art/audio fallbacks).
- Boot: title screen + main menu renders; mouse/keyboard input works; options dialog opens/closes.
- Start: “Start new game” launches a real scenario (loads scenario INI, theater, SHPs, palettes, strings).
- Play: selection, orders, sidebar/radar, EVA/SFX, and game-speed timing behave sensibly.
- Finish: win/lose/restart flows work and return to menu; quit returns to desktop.

## Playable single-player (critical path)
Status: Next steps. Scope: make the above definition true before chasing deep parity details.
- Finish Win95 main menu/game selection parity: new game/campaign progression and a real load-mission entry point (`Select_Game()`) (`src/port_runtime.cpp`, `src/loaddlg.cpp`).
- Audit remaining UI screens for canonical `source.Blit(dest)` ordering as modules are ported (`src/include/legacy/wwlib32.h`, `src/wwlib_runtime.cpp`).

## Save/load and profile persistence
Status: Next steps. Scope: allow a normal play session to be resumed and options to persist.
- Complete the full save/load UI flow parity against Win95 (load/save/delete mission behavior, description rules, error handling, save-game dialog behavior) (`src/loaddlg.cpp`, `src/saveload.cpp`, `src/port_runtime.cpp`).
- Verify save/profile writes hit disk and surface errors correctly (`RawFileClass::Error()` parity) (RawFileClass retry/exit prompts are now ported; still need in-game verification) (`src/rawfile.cpp`, `src/ccfile.cpp`, `src/options.cpp`, `src/game.cpp`).
- Replace the minimal `CONQUER.INI` auto-generation with a real SETUP/config flow while keeping the repo-local `CD/...` mirror working (`src/port_setup.cpp`, `src/options.cpp`).

## Audio and movie parity (needed for “feels like C&C”)
Status: Next steps. Scope: audio + movies are required for a “real game” experience even if not strictly required to click units.
- Finish SDL audio parity: mixer behavior (priority/channel reservation, pan law, fades/stops) and music/theme streaming should match Win95 and obey menu sliders (`src/audio_play.cpp`, `src/theme.cpp`, `src/options.cpp`).
- Add VQA audio playback + subtitle/EVA timing and verify skip rules/centering/cropping against Win95 (`src/movie.cpp`, `src/vqa_decoder.cpp`).

## Multiplayer and networking
Status: Next steps. Scope: restore Win95 multiplayer flows once single-player is playable.
- Complete the remaining TCP/IP + session behavior (timeouts, lobby dialogs, determinism checks) while keeping the UDP-backed IPX95 path (`src/tcpip.cpp`, `src/connect.cpp`).
- Keep the portable CCDDE replacement aligned with Win95 WChat expectations and document the integration knobs (`src/ccdde.cpp`, `src/dde.cpp`).

## Clean-up (remove remaining fallbacks)
Status: Next steps. Scope: eliminate “it runs but isn’t canonical” behaviors once playability is proven.
- Port the map editor entry points (`Map_Edit_Loop`, map selection flows) so `GameToPlay == GAME_MAP_EDIT` matches Win95 behavior instead of relying on the current minimal loop (`src/maingame.cpp`, `src/port_runtime.cpp`).
- Audit remaining “skeleton” translation units and retire them only after behavior-complete ports exist.

## Testing and parity verification
Status: Next steps. Scope: keep the port regressions visible and the docs accurate.
- Add focused smoke tests/harnesses for: MIX registration order, scenario load, palette fades, and basic input (a 60-second “run a mission loop” headless mode would be ideal).
- Verify `Build_Frame` big/theater caching path against Win95: exercise WSA/SHP-heavy UI screens and switch theaters to ensure `Reset_Theater_Shapes()` behaves correctly (`src/keyframe_helpers.cpp`, `src/scenario.cpp`).
- For debugging convenience: enable pause-on-fatal when launching outside a terminal (currently controlled via an environment variable), or use the autostart-scenario mode (e.g. `SCG01EA`, also currently env-driven) with the optional load-only/load-title/draw-once/frame-limit controls.
- When verifying palette fades, include `Fade_Palette_To(..., ..., NULL)` call sites (Win95 still shows the fade via hardware palette updates) and compare the fade duration against Win95.
- Add quick visual regression checks for text rendering and title/menu palette correctness (`src/text.cpp`, `src/load_title.cpp`, `src/wwlib_runtime.cpp`).
- When debugging startup hangs, run with `--verbose` and capture stderr; if it hangs at `C&C95 - In Read_Scenario.`, the trace now prints progress through `Clear_Scenario()` and `Read_Scenario_Ini()` (including scenario INI filename + `CCFileClass` availability/size/read).
- Keep `PROGRESS.md` and this file in sync (remove stale “stubbed” notes once the corresponding module is fully ported).

## Recently completed (for context)
Status: Reference. Scope: do not add new work here; keep short.
Implementation done!: Restored textured UI box rendering (`Draw_Box`/`CC_Texture_Fill`) so menus/dialogs use `BTEXTURE.SHP` (`src/dialog.cpp`).
Implementation done!: Restored the legacy choose-side animation flow and briefing playback (`src/intro.cpp`).
Implementation done!: Replaced the timing-only movie stub with a real classic 8-bit VQA decoder and frame blitter (`src/movie.cpp`, `src/vqa_decoder.cpp`).
Implementation done!: Implemented the `WWMouseClass` draw/erase overlay so the SDL path shows the legacy cursor (`src/wwlib_runtime.cpp`).
