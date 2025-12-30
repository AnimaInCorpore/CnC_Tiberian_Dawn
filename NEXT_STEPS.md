# Next steps to port the project
Tackle one chunk at a time; when a chunk has no remaining next steps, mark it with "Implementation done!".

## What “playable” means (single-player)
Status: Definition. Scope: the SDL build should let a user boot to the main menu, start a mission, play it to a win/lose, and exit cleanly using only original game assets (no built-in art/audio fallbacks).
- Boot: title screen + main menu renders; mouse/keyboard input works; options dialog opens/closes.
- Start: “Start new game” launches a real scenario (loads scenario INI, theater, SHPs, palettes, strings).
- Play: selection, orders, sidebar/radar, EVA/SFX, and game-speed timing behave sensibly.
- Finish: win/lose/restart flows work and return to menu; quit returns to desktop.

## Playable single-player (critical path)
Status: Next steps. Scope: make the above definition true before chasing deep parity details.
 - Implementation done!: MIX bootstrap registers scenario/theater/audio/movie archives (incl. `SC*.MIX`, `TEMPERAT.MIX`, `SOUNDS.MIX`, `AUD.MIX`, `SCORES.MIX`, `MOVIES.MIX`) from the repo-local `CD/...` mirrors (`src/game.cpp`, `src/load_title.cpp`).
- Implementation done!: Fail-fast startup validation when the required repo-local `CD/...` asset mirror is missing (`src/port_runtime.cpp`, `src/cdfile.cpp`).
- Verify all remaining UI screens use canonical `source.Blit(dest)` ordering (avoid accidental `dest.Blit(source)` inversions) as modules are ported (`src/include/legacy/wwlib32.h`, `src/wwlib_runtime.cpp`).
- Replicate the remaining Win95 `Select_Game()` behavior: new game/campaign progression and a real load-mission entry point (bonus/expansion routing is now implemented) (`src/port_runtime.cpp`, `src/loaddlg.cpp`).
- Implementation done!: Tightened SDL main-loop timing so game speed and timers behave consistently: `Options.GameSpeed` again controls the frame cadence (not milliseconds), and `TimerClass::Time()` returns Win95-style 60Hz ticks; focus loss pauses simulation updates (`src/port_runtime.cpp`, `src/wwlib_runtime.cpp`, `src/include/legacy/wwlib32.h`).
- Implementation done!: In-game options overlay now persists settings (restored `Options.Save_Settings()` on resume) so sliders/toggles survive returning to gameplay (`src/goptions.cpp`).
- Verify `CDFileClass` search-drive probing tries all candidate mirrors before prompting to retry (avoid getting stuck retrying a missing `CD2/...` path when the asset lives on `CD1/...`) (`src/cdfile.cpp`).

## Save/load and profile persistence
Status: Next steps. Scope: allow a normal play session to be resumed and options to persist.
- Verify load/save/delete mission flows against Win95: list ordering, description editing rules, and error handling (`src/loaddlg.cpp`, `src/saveload.cpp`, `src/port_runtime.cpp`).
- Port the corresponding save-game dialog and ensure save slots + descriptions match Win95 (no new dependencies; pick a platform-appropriate writable directory and document it).
- Verify save/profile writes hit disk correctly now that `RawFileClass::Open()` again creates/truncates files on `WRITE`, and that file errors surface via `RawFileClass::Error()` instead of silently failing (`src/rawfile.cpp`, `src/ccfile.cpp`, `src/options.cpp`, `src/game.cpp`).
- Implementation done!: `CONQUER.INI` reads/writes now resolve to the same path (prefer working directory, else SDL per-user pref path) so `OptionsClass::Save_Settings()` persists to the config location that startup uses (`src/port_paths.cpp`, `src/options.cpp`, `src/game.cpp`, `src/port_runtime.cpp`, `src/port_setup.cpp`).
- Replace the minimal `CONQUER.INI` auto-generation with a real SETUP/config flow (INI parsing + persistence) while keeping the repo-local `CD/...` mirror working (`src/port_setup.cpp`, `src/options.cpp`).

## Audio and movie parity (needed for “feels like C&C”)
Status: Next steps. Scope: audio + movies are required for a “real game” experience even if not strictly required to click units.
- Finish SDL audio parity: mixer behavior (priority/channel reservation, pan law, fades/stops) and music/theme streaming should match Win95 and obey menu sliders (`src/audio_play.cpp`, `src/theme.cpp`, `src/options.cpp`).
- Implementation done!: Implemented the Sound/Visual Controls dialog backends so the in-game UI controls now affect runtime state (`src/sounddlg.cpp`, `src/visudlg.cpp`).
- Add VQA audio playback + subtitle/EVA timing (current `Play_Movie()` decodes video + palette only) and verify skip rules/centering/cropping against Win95 (`src/movie.cpp`, `src/vqa_decoder.cpp`).
- Match Win95 gradient UI text: implement `TPF_USE_GRAD_PAL` shading using the shipped gradient resources (`GRAD6FNT.FNT` palette + `12GRNGRD.FNT` blend table) in the SDL text renderer (`src/text.cpp`).
- Verify `Set_Font_Palette`/ColorXlat behavior (including `TBLACK` transparency and the base color indices) matches Win95 across title/menu and in-game palettes (`src/include/legacy/compat.h`, `src/wwlib_runtime.cpp`, `src/text.cpp`).

## Multiplayer and networking
Status: Next steps. Scope: restore Win95 multiplayer flows once single-player is playable.
- Implementation done!: Ported packet serialization and endianness helpers for the multiplayer event stream (`src/packet.cpp`, `src/field.cpp`).
- Complete the remaining TCP/IP + session behavior (timeouts, lobby dialogs, determinism checks) while keeping the UDP-backed IPX95 path (`src/tcpip.cpp`, `src/connect.cpp`).
- Keep the portable CCDDE replacement aligned with Win95 WChat expectations and document the integration knobs (`src/ccdde.cpp`, `src/dde.cpp`).

## Clean-up (remove remaining fallbacks)
Status: Next steps. Scope: eliminate “it runs but isn’t canonical” behaviors once playability is proven.
- Remove palette/animation fallback paths and match Win95 interpolation/animation timing (`src/interpal_fallback.cpp`, `src/interpal.cpp`).
- Replace the dummy CD probe in `src/include/legacy/getcd.h` with a real implementation (or remove the Win95-only code paths entirely if the SDL/data-dir path is canonical).
- Port the map editor entry points (`Map_Edit_Loop`, map selection flows) so `GameToPlay == GAME_MAP_EDIT` matches Win95 behavior instead of relying on the current minimal loop (`src/maingame.cpp`, `src/port_runtime.cpp`).
- Ensure radar mini-icon generation uses the canonical `Small_Icon()` sampling behavior wherever iconsets are used for mini-map display (`src/jshell.cpp`, `src/wwlib_runtime.cpp`).
- Audit remaining “skeleton” translation units (e.g. `src/loaddlg.cpp`) and retire them only after behavior-complete ports exist.

## Testing and parity verification
Status: Next steps. Scope: keep the port regressions visible and the docs accurate.
- Add focused smoke tests/harnesses for: MIX registration order, scenario load, palette fades, and basic input (a 60-second “run a mission loop” headless mode would be ideal).
- When verifying palette fades, include `Fade_Palette_To(..., ..., NULL)` call sites (Win95 still shows the fade via hardware palette updates) and compare the fade duration against Win95.
- Add a quick visual regression check for text rendering (shadow/background fill and palette index 0 behavior) (`src/text.cpp`).
- Add a quick visual regression check for title/menu palette correctness (title art + green dialog texture colors, and ensure `Set_Font_Palette` does not mutate the screen palette) (`src/load_title.cpp`, `src/wwlib_runtime.cpp`).
- When debugging startup hangs, run with `--verbose` (or `TD_VERBOSE=1`) and capture stderr; the port now logs SDL video/render driver selection plus menu/scenario startup milestones.
- If it hangs at `C&C95 - In Read_Scenario.`, the verbose trace now prints progress through `Clear_Scenario()` and `Read_Scenario_Ini()` (including scenario INI filename + `CCFileClass` availability/size/read).
- Keep `PROGRESS.md` and this file in sync (remove stale “stubbed” notes once the corresponding module is fully ported).

## Recently completed (for context)
Status: Reference. Scope: do not add new work here; keep short.
Implementation done!: Restored textured UI box rendering (`Draw_Box`/`CC_Texture_Fill`) so menus/dialogs use `BTEXTURE.SHP` (`src/dialog.cpp`).
Implementation done!: Replaced the placeholder intro handler with a working GDI/Nod side selection (`src/intro_port.cpp`).
Implementation done!: Replaced the timing-only movie stub with a real classic 8-bit VQA decoder and frame blitter (`src/movie.cpp`, `src/vqa_decoder.cpp`).
Implementation done!: Implemented the `WWMouseClass` draw/erase overlay so the SDL path shows the legacy cursor (`src/wwlib_runtime.cpp`).
