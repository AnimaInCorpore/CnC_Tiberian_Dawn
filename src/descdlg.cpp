/*
** Description dialog port (partial)
*/

#include "function.h"
#include "descdlg.h"

void DescriptionClass::Process(char *string)
{
    Set_Window(WINDOW_EDITOR, OPTION_X, OPTION_Y, OPTION_WIDTH, OPTION_HEIGHT);
    Set_Logic_Page(SeenBuff);

    TextButtonClass optionsbtn(
        BUTTON_OPTIONS,
        TXT_OK,
        TPF_6PT_GRAD,
        0,
        BUTTON_Y);

    TextButtonClass cancelbtn(
        BUTTON_CANCEL,
        TXT_CANCEL,
        TPF_6PT_GRAD,
        0,
        BUTTON_Y);

    cancelbtn.X  = OPTION_X + ((OPTION_WIDTH - optionsbtn.Width)/3)*2;
    optionsbtn.X = OPTION_X + ((OPTION_WIDTH - optionsbtn.Width)/3);
    optionsbtn.Add_Tail(cancelbtn);

    EditClass edit(
        BUTTON_EDIT,
        string,
        31,
        TPF_6PT_GRAD,
        0,
        EDIT_Y,
        EDIT_W);

    edit.Set_Focus();
    edit.X = OPTION_X + (OPTION_WIDTH - edit.Width)/2;
    optionsbtn.Add_Tail(edit);

    GadgetClass dialog(OPTION_X, OPTION_Y, OPTION_WIDTH, OPTION_HEIGHT, GadgetClass::LEFTPRESS);
    optionsbtn.Add_Tail(dialog);

    ControlClass background(BUTTON_OPTIONS, 0, 0, SeenBuff.Get_Width(), SeenBuff.Get_Height(), GadgetClass::LEFTPRESS|GadgetClass::RIGHTPRESS);
    optionsbtn.Add_Tail(background);

    bool display = true;
    bool process = true;
    while (process) {
        if (AllSurfaces.SurfacesRestored) {
            AllSurfaces.SurfacesRestored = FALSE;
            display = true;
        }

        Call_Back();

        if (display) {
            Hide_Mouse();
            Window_Box(WINDOW_EDITOR, BOXSTYLE_GREEN_BORDER);
            Draw_Caption(TXT_MISSION_DESCRIPTION, OPTION_X, OPTION_Y, OPTION_WIDTH);
            optionsbtn.Draw_All();
            Show_Mouse();
            display = false;
        }

        KeyNumType input = optionsbtn.Input();

        switch (input) {
            case KN_RETURN:
            case BUTTON_OPTIONS|KN_BUTTON:
                strtrim(string);
                process = false;
                break;

            case KN_ESC:
            case BUTTON_CANCEL|KN_BUTTON:
                string[0] = '\0';
                strtrim(string);
                process = false;
                break;

            case BUTTON_EDIT|KN_BUTTON:
                break;
        }
    }
}
