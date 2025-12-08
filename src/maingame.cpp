#include <SDL.h>

#include "legacy/ccdde.h"
#include "legacy/defines.h"
#include "legacy/event.h"
#include "legacy/externs.h"
#include "legacy/function.h"
#include "legacy/goptions.h"
#include "legacy/jshell.h"
#include "legacy/map_shim.h"
#include "legacy/wwlib32.h"

// Legacy functions declared here in the original file.
bool Map_Edit_Loop(void);

// Legacy flags and counters pulled in from the wider game state.
extern bool ReadyToQuit;
int TotalLocks = 0;

// Legacy routines that haven't been surfaced through a modern header yet.
void Register_Game_Start_Time(void);
void Send_Statistics_Packet(void);

#ifdef FORCE_WINSOCK
extern bool Spawn_WChat(bool can_launch);
extern bool SpawnedFromWChat;
#endif

namespace {
void Push_Quit_Event() {
    SDL_Event quit_event{};
    quit_event.type = SDL_QUIT;
    SDL_PushEvent(&quit_event);
}
}  // namespace

// Global flag the legacy runtime flips while inside the main loop.
bool InMainLoop = false;

void Main_Game(int argc, char* argv[]) {
    bool fade = false;

    if (!Init_Game(argc, argv)) {
        ReadyToQuit = true;
        Push_Quit_Event();
        return;
    }

    CCDebugString("C&C95 - Game initialisation complete.\n");

    /*
    **	Game processing loop:
    **	1) Select which game to play, or whether to exit (don't fade the palette
    **		on the first game selection, but fade it in on subsequent calls)
    **	2) Invoke either the main-loop routine, or the editor-loop routine,
    **		until they indicate that the user wants to exit the scenario.
    */
    while (!ReadyToQuit && Select_Game(fade)) {
        ScenarioInit = 0;  // Kludge.

        fade = true;

        /*
        **	Make the game screen visible, clear the keyboard buffer of spurious
        **	values, and then show the mouse.  This PRESUMES that Select_Game() has
        **	told the map to draw itself.
        */
        Fade_Palette_To(GamePalette, FADE_PALETTE_MEDIUM, nullptr);
        Keyboard::Clear();

        /*
        ** Only show the mouse if we're not playing back a recording.
        */
        if (PlaybackGame) {
            Hide_Mouse();
        } else {
            Show_Mouse();
        }

        SpecialDialog = SDLG_NONE;
        if (GameToPlay == GAME_INTERNET) {
            Register_Game_Start_Time();
            GameStatisticsPacketSent = false;
            PacketLater = nullptr;
            ConnectionLost = false;
        } else {
#ifdef WIN32
            DDEServer.Disable();
#endif
        }

        InMainLoop = true;

#ifdef SCENARIO_EDITOR
        /*
        **	Scenario-editor version of main-loop processing
        */
        for (;;) {
            /*
            **	Non-scenario-editor-mode: call the game's main loop
            */
            if (!Debug_Map) {
                TotalLocks = 0;
                if (Main_Loop()) {
                    break;
                }

                if (SpecialDialog != SDLG_NONE) {
                    switch (SpecialDialog) {
                        case SDLG_SPECIAL:
                            Map.Help_Text(TXT_NONE);
                            Map.Override_Mouse_Shape(MOUSE_NORMAL, false);
                            Special_Dialog();
                            SpecialDialog = SDLG_NONE;
                            break;

                        case SDLG_OPTIONS:
                            Map.Help_Text(TXT_NONE);
                            Map.Override_Mouse_Shape(MOUSE_NORMAL, false);
                            Options.Process();
                            SpecialDialog = SDLG_NONE;
                            break;

                        case SDLG_SURRENDER:
                            Map.Help_Text(TXT_NONE);
                            Map.Override_Mouse_Shape(MOUSE_NORMAL, false);
                            if (Surrender_Dialog()) {
                                OutList.Add(EventClass(EventClass::DESTRUCT));
                            }
                            SpecialDialog = SDLG_NONE;
                            break;

                        default:
                            break;
                    }
                }
            } else {
                /*
                **	Scenario-editor-mode: call the editor's main loop
                */
                if (Map_Edit_Loop()) {
                    break;
                }
            }
        }
#else
        /*
        **	Non-editor version of main-loop processing
        */
        for (;;) {
            /*
            **	Call the game's main loop
            */
            TotalLocks = 0;
            if (Main_Loop()) {
                break;
            }

            /*
            **	If the SpecialDialog flag is set, invoke the given special dialog.
            **	This must be done outside the main loop, since the dialog will call
            **	Main_Loop(), allowing the game to run in the background.
            */
            if (SpecialDialog != SDLG_NONE) {
                switch (SpecialDialog) {
                    case SDLG_SPECIAL:
                        Map.Help_Text(TXT_NONE);
                        Map.Override_Mouse_Shape(MOUSE_NORMAL, false);
                        Special_Dialog();
                        SpecialDialog = SDLG_NONE;
                        break;

                    case SDLG_OPTIONS:
                        Map.Help_Text(TXT_NONE);
                        Map.Override_Mouse_Shape(MOUSE_NORMAL, false);
                        Options.Process();
                        SpecialDialog = SDLG_NONE;
                        break;

                    case SDLG_SURRENDER:
                        Map.Help_Text(TXT_NONE);
                        Map.Override_Mouse_Shape(MOUSE_NORMAL, false);
                        if (Surrender_Dialog()) {
                            OutList.Add(EventClass(EventClass::DESTRUCT));
                        }
                        SpecialDialog = SDLG_NONE;
                        break;

                    default:
                        break;
                }
            }
        }
#endif
        InMainLoop = false;

        if (ReadyToQuit) {
            break;
        }

        if (!GameStatisticsPacketSent && PacketLater) {
            Send_Statistics_Packet();
        }

        /*
        **	Scenario is done; fade palette to black
        */
        Fade_Palette_To(BlackPalette, FADE_PALETTE_SLOW, nullptr);
        VisiblePage.Clear();

#ifndef DEMO
        /*
        **	Un-initialize whatever needs it, for each game played.
        **
        **	Shut down either the modem or network; they'll get re-initialized if
        **	the user selections those options again in Select_Game().  This
        **	"re-boots" the modem & network code, which I currently feel is safer
        **	than just letting it hang around.
        ** (Skip this step if we're in playback mode; the modem or net won't have
        ** been initialized in that case.)
        */
        if ((RecordGame && !SuperRecord) || PlaybackGame) {
            RecordFile.Close();
        }

        if (!PlaybackGame) {
            switch (GameToPlay) {
                case GAME_NULL_MODEM:
                case GAME_MODEM:
                    Modem_Signoff();
                    break;

                case GAME_IPX:
                    Shutdown_Network();
                    break;

                case GAME_INTERNET:
                    break;

                default:
                    break;
            }
        }

        /*
        **	If we're playing back, the mouse will be hidden; show it.
        ** Also, set all variables back to normal, to return to the main menu.
        */
        if (PlaybackGame) {
            Show_Mouse();
            GameToPlay = GAME_NORMAL;
            PlaybackGame = 0;
        }

#ifdef FORCE_WINSOCK
        /*
        ** If we were spawned from WChat then dont go back to the main menu - just quit
        **
        ** New: If spawned from WChat then maximise WChat and go back to the main menu after all
        */
        if (Special.IsFromWChat) {
            Shutdown_Network();  // Clear up the pseudo IPX stuff
            Winsock.Close();
            Special.IsFromWChat = false;
            SpawnedFromWChat = false;
            DDEServer.Delete_MPlayer_Game_Info();  // Make sure we dont use the same start packet twice
            GameToPlay = GAME_NORMAL;  // Have to do this or we will got straight to the multiplayer menu
            Spawn_WChat(false);  // Will switch back to Wchat. It must be there because its been poking us
            // break;
        }
#endif  // FORCE_WINSOCK

#endif  // DEMO
    }

#ifndef DEMO
    /*
    **	Free the scenario description buffers
    */
    Free_Scenario_Descriptions();
#endif

#ifndef NOMEMCHECK
    Uninit_Game();
#endif

    ReadyToQuit = true;
    Push_Quit_Event();
}
