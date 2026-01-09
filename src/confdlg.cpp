/*
**  Command & Conquer(tm)
**  Copyright 2025 Electronic Arts Inc.
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "confdlg.h"

#include "legacy_compat.h"

bool ConfirmationClass::Process(int text) { return Process(Text_String(text)); }

bool ConfirmationClass::Process(char const* string) {
    int factor = (SeenBuff.Get_Width() == 320) ? 1 : 2;

    enum { NUM_OF_BUTTONS = 2 };

    char buffer[80 * 3];
    bool result = true;
    int width = 0;
    int bwidth;
    int bheight;
    int height = 0;
    int selection = 0;
    bool pressed;
    int curbutton;
    TextButtonClass* buttons[NUM_OF_BUTTONS];

    if (!string) string = "";

    buffer[0] = '\0';
    std::strncpy(buffer, string, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    Fancy_Text_Print(TXT_NONE, 0, 0, TBLACK, TBLACK, TPF_6PT_GRAD | TPF_NOSHADOW);
    Format_Window_String(buffer, 200 * factor, width, height);
    width += 60 * factor;
    height += 60 * factor;
    int x = (320 * factor - width) / 2;
    int y = (200 * factor - height) / 2;

    Set_Logic_Page(SeenBuff);

    bheight = FontHeight + FontYSpacing + 2;
    bwidth = MAX((String_Pixel_Width(Text_String(TXT_YES)) + 8), 30);

    TextButtonClass yesbtn(BUTTON_YES, TXT_YES, TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW,
                           x + 10 * factor, y + height - (bheight + 5 * factor), bwidth);

    TextButtonClass nobtn(BUTTON_NO, TXT_NO, TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW,
                          x + width - (bwidth + 10 * factor),
                          y + height - (bheight + 5 * factor), bwidth);

    nobtn.Add_Tail(yesbtn);

    curbutton = 1;
    buttons[0] = &yesbtn;
    buttons[1] = &nobtn;
    buttons[curbutton]->Turn_On();

    GadgetClass dialog(x, y, width, height, GadgetClass::LEFTPRESS);
    dialog.Add_Tail(yesbtn);

    ControlClass background(BUTTON_NO, 0, 0, SeenBuff.Get_Width(), SeenBuff.Get_Height(),
                            GadgetClass::LEFTPRESS | GadgetClass::RIGHTPRESS);
    background.Add_Tail(yesbtn);

    bool display = true;
    bool process = true;
    pressed = false;
    while (process) {
        if (GameToPlay == GAME_NORMAL) {
            Call_Back();
        } else {
            if (Main_Loop()) {
                process = false;
                result = false;
            }
        }

        if (AllSurfaces.SurfacesRestored) {
            AllSurfaces.SurfacesRestored = FALSE;
            display = TRUE;
        }

        if (display) {
            Hide_Mouse();

            Dialog_Box(x, y, width, height);
            Draw_Caption(TXT_CONFIRMATION, x, y, width);
            Fancy_Text_Print(buffer, x + 20 * factor, y + 30 * factor, CC_GREEN, TBLACK,
                             TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW);

            yesbtn.Draw_All();
            Show_Mouse();
            display = false;
        }

        KeyNumType input = yesbtn.Input();

        switch (input) {
            case (BUTTON_YES | KN_BUTTON):
                selection = BUTTON_YES;
                pressed = true;
                break;

            case (KN_ESC):
            case (BUTTON_NO | KN_BUTTON):
                selection = BUTTON_NO;
                pressed = true;
                break;

            case (KN_LEFT):
                buttons[curbutton]->Turn_Off();
                buttons[curbutton]->Flag_To_Redraw();

                curbutton--;
                if (curbutton < 0) curbutton = NUM_OF_BUTTONS - 1;

                buttons[curbutton]->Turn_On();
                buttons[curbutton]->Flag_To_Redraw();
                break;

            case (KN_RIGHT):
                buttons[curbutton]->Turn_Off();
                buttons[curbutton]->Flag_To_Redraw();

                curbutton++;
                if (curbutton > (NUM_OF_BUTTONS - 1)) curbutton = 0;

                buttons[curbutton]->Turn_On();
                buttons[curbutton]->Flag_To_Redraw();
                break;

            case (KN_RETURN):
                selection = curbutton + BUTTON_YES;
                pressed = true;
                break;

            default:
                break;
        }

        if (pressed) {
            switch (selection) {
                case (BUTTON_YES):
                    result = true;
                    process = false;
                    break;

                case (BUTTON_NO):
                    result = false;
                    process = false;
                    break;
            }
            pressed = false;
        }
    }

    return result;
}
