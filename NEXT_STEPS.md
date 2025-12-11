# Next Steps Toward Main Menu Parity

- Finish the remaining bootstrap work in `src/port_stubs.cpp` by pulling over the real `STARTUP.CPP`/`CONQUER.CPP` init/shutdown steps (heap sizing, mix/font preloads, intro handling, mouse/setup probes, and the network/DDE teardown paths beyond the new timer/DDE scaffolding).
- Port the full main menu logic from `MENUS.CPP` instead of the simplified SDL menu in `src/menus.cpp`, including intro/movie gating, load/save handling, options/net dialogs, and timeout behavior.
- Restore display/render scaffolding: implement `DisplayClass::Draw_It/AI` and map helpers in `src/display.cpp`; fill out `GScreenClass` behaviors in `src/gscreen.cpp`; replace `MapStubClass` and `MapClass` stubs (`src/map_shim.cpp`, `src/gameplay_core_stub.cpp`) so rendering, cursor, and radar work as in Win95.
- Reintroduce the startup option/UI plumbing: implement `GameOptionsClass::Adjust_Variables_For_Resolution/Process`, Sidebar/Tab/Radio/Theme behaviors, and the related UI helpers in `src/gameplay_class_stubs.cpp` so startup can adjust resolution and build the HUD correctly.
- Replace message/audio placeholders: wire `CCMessageBox::Process` and sound effect entry points in `src/linker_stubs.cpp`; expand `src/audio_stub.cpp` to mix/stream like `AUDIO.CPP` or a faithful SDL equivalent.
- Audit `src/game.cpp` against `STARTUP.CPP` to restore missing prep steps (swap-file cleanup, full MMX/CD probing, config handling) and ensure `Game_Startup` mirrors the canonical Win95 sequencing before showing the menu.
