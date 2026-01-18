#include "menus.h"

#include "function.h"
#include "gadget.h"
#include "sdl_platform.h"

enum MenuButtonId {
    BUTTON_START = 200,
    BUTTON_LOAD = 201,
    BUTTON_OPTIONS = 202,
    BUTTON_EXIT = 203
};

int Main_Menu(unsigned long timeout)
{
    Set_Logic_Page(HidPage);

    const int screen_w = SeenBuff.Get_Width();
    const int screen_h = SeenBuff.Get_Height();
    const int factor = (screen_w == 320) ? 1 : 2;

    const int dialog_w = 170 * factor;
    const int dialog_h = 150 * factor;
    const int dialog_x = (screen_w - dialog_w) / 2;
    const int dialog_y = (screen_h - dialog_h) / 2;

    const int button_w = dialog_w - 20 * factor;
    const int button_x = dialog_x + 10 * factor;
    const int button_h = FontHeight + FontYSpacing + 2;
    const int button_step = button_h + 10 * factor;
    const int first_button_y = dialog_y + 28 * factor;

    TextButtonClass startbtn(BUTTON_START, TXT_START_NEW_GAME,
                             TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW, button_x,
                             first_button_y + 0 * button_step, button_w);
    TextButtonClass loadbtn(BUTTON_LOAD, TXT_LOAD_MISSION,
                            TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW, button_x,
                            first_button_y + 1 * button_step, button_w);
    TextButtonClass optionsbtn(BUTTON_OPTIONS, TXT_OPTIONS,
                               TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW, button_x,
                               first_button_y + 2 * button_step, button_w);
    TextButtonClass exitbtn(BUTTON_EXIT, TXT_EXIT_GAME,
                            TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW, button_x,
                            first_button_y + 3 * button_step, button_w);

    startbtn.Add_Tail(loadbtn);
    startbtn.Add_Tail(optionsbtn);
    startbtn.Add_Tail(exitbtn);

    TextButtonClass* buttons[4];
    buttons[0] = &startbtn;
    buttons[1] = &loadbtn;
    buttons[2] = &optionsbtn;
    buttons[3] = &exitbtn;

    int curbutton = 0;
    buttons[curbutton]->Turn_On();

    unsigned long starttime = SDL_Platform_Ticks();
    bool display = true;
    bool process = true;
    int retval = -1;

    while (process) {
        bool should_quit = false;
        SDL_Platform_Pump_Events(should_quit);
        if (should_quit) {
            retval = 3;
            break;
        }

        if (timeout && (SDL_Platform_Ticks() - starttime) > timeout) {
            retval = -1;
            break;
        }

        KeyNumType input = startbtn.Input();
        switch (input) {
            case (BUTTON_START | KN_BUTTON):
                retval = 0;
                process = false;
                break;
            case (BUTTON_LOAD | KN_BUTTON):
                retval = 1;
                process = false;
                break;
            case (BUTTON_OPTIONS | KN_BUTTON):
                retval = 2;
                process = false;
                break;
            case (BUTTON_EXIT | KN_BUTTON):
                retval = 3;
                process = false;
                break;
            case KN_ESC:
                retval = 3;
                process = false;
                break;
            case KN_UP:
                buttons[curbutton]->Turn_Off();
                curbutton--;
                if (curbutton < 0) curbutton = 3;
                buttons[curbutton]->Turn_On();
                display = true;
                break;
            case KN_DOWN:
                buttons[curbutton]->Turn_Off();
                curbutton++;
                if (curbutton > 3) curbutton = 0;
                buttons[curbutton]->Turn_On();
                display = true;
                break;
            case KN_RETURN:
                retval = curbutton;
                process = false;
                break;
            default:
                break;
        }

        if (display) {
            CC_Texture_Fill(NULL, 0, 0, 0, screen_w, screen_h);
            Dialog_Box(dialog_x, dialog_y, dialog_w, dialog_h);
            Fancy_Text_Print("Main Menu", (unsigned)(dialog_x + dialog_w / 2),
                             (unsigned)(dialog_y + 8 * factor), CC_GREEN, TBLACK,
                             TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW);
            startbtn.Draw_All(true);
            display = false;
        }

        HidPage.Blit(SeenBuff);
        SDL_Platform_Present_Indexed8(SeenBuff.Data(), screen_w, screen_h, SeenBuff.Pitch());
        SDL_Platform_Delay(16);
    }

    return retval;
}
