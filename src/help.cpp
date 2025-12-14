/*
**	Command & Conquer(tm)
**	Copyright 2025 Electronic Arts Inc.
*/

#include "legacy/function.h"

short const HelpClass::OverlapList[30] = {
    REFRESH_EOL
};

char const * HelpClass::HelpText;

CountDownTimerClass HelpClass::CountDownTimer;

HelpClass::HelpClass(void)
{
    X = 0;
    Y = 0;
    Width = 0;
    Text = TXT_NONE;
    Color = LTGREY;
    CountDownTimer.Set(0);
    IsRight = false;
    Cost = 0;
}

void HelpClass::Init_Clear(void)
{
    TabClass::Init_Clear();
    Set_Text(TXT_NONE);
}

short const * HelpClass::Overlap_List(void) const
{
    if (Text == TXT_NONE || CountDownTimer.Time()) {
        ((short &)(OverlapList[0])) = REFRESH_EOL;
    }
    return(OverlapList);
}

void HelpClass::AI(KeyNumType &key, int x, int y)
{
    if (!CountDownTimer.Time() && !IsRight && (x != X || y != Y)) {
        Help_Text(TXT_NONE);
    }

    if (CountDownTimer.Time() && !HelpText && Text != TXT_NONE) {
        if (!IsRight && (X != x || Y != y)) {
            X = x;
            Y = y;
            CountDownTimer.Set(HELP_DELAY);
            Set_Text(TXT_NONE);
        } else {
            Set_Text(Text);
        }
    }

    TabClass::AI(key, x, y);
}

void HelpClass::Help_Text(int text, int x, int y, int color, bool quick, int cost)
{
    if (text != Text) {
        if (Text != TXT_NONE) {
            Refresh_Cells(Coord_Cell(TacticalCoord), &OverlapList[0]);
        }

        X = x;
        if (x == -1) X = Get_Mouse_X();
        Y = y;
        if (y == -1) Y = Get_Mouse_Y();
        IsRight = (y != -1) || (x != -1);

        if (quick) {
            CountDownTimer.Set(1);
        } else {
            CountDownTimer.Set(HELP_DELAY);
        }

        Color = color;
        Text = text;
        Cost = cost;
    }
}

void HelpClass::Draw_It(bool forced)
{
    TabClass::Draw_It(forced);

    if (Text != TXT_NONE && (forced || !CountDownTimer.Time())) {
        if (LogicPage->Lock()){
            Fancy_Text_Print(Text, DrawX, DrawY, Color, BLACK, TPF_MAP|TPF_NOSHADOW);
            LogicPage->Draw_Rect(DrawX-1, DrawY-1, DrawX+Width+1, DrawY+FontHeight, Color);

            if (Cost) {
                char buffer[15];
                sprintf(buffer, "$%d", Cost);
                int width = String_Pixel_Width(buffer);
                Fancy_Text_Print(buffer, DrawX, DrawY+FontHeight, Color, BLACK, TPF_MAP|TPF_NOSHADOW);
                LogicPage->Draw_Rect(DrawX-1, DrawY+FontHeight, DrawX+width+1, DrawY+FontHeight+FontHeight-1, Color);
                LogicPage->Draw_Line(DrawX, DrawY+FontHeight, DrawX+MIN(width+1, Width) - 1, DrawY+FontHeight, BLACK);
            }

            LogicPage->Unlock();
        }
    }
}

void HelpClass::Set_Text(int text)
{
    if (text != TXT_NONE) {
        Text = text;
        Fancy_Text_Print(TXT_NONE, 0, 0, 0, 0, TPF_MAP|TPF_NOSHADOW);
        Width = String_Pixel_Width(Text_String(Text));
        if (IsRight) {
            DrawX = X - Width;
            DrawY = Y;
        } else {
            int right = TacPixelX + Lepton_To_Pixel(TacLeptonWidth) - 3;
            int bottom = TacPixelY + Lepton_To_Pixel(TacLeptonHeight) - 1;

            DrawX = X+X_OFFSET;
            DrawY = Y+Y_OFFSET;
            if (DrawX + Width > right) {
                DrawX -= (DrawX+Width) - right;
            }
            if (DrawY + FontHeight > bottom) {
                DrawY -= (DrawY+FontHeight) - bottom;
            }
            if (DrawX < TacPixelX+1) DrawX = TacPixelX+1;
            if (DrawY < TacPixelY+1) DrawY = TacPixelY+1;
        }
        int lines = (Cost) ? 2 : 1;
        memcpy((void*)OverlapList, Text_Overlap_List(Text_String(Text), DrawX-1, DrawY, lines), sizeof(OverlapList));
    }
}

bool HelpClass::Scroll_Map(DirType facing, int & distance, bool really)
{
    if (really) {
        Help_Text(TXT_NONE);
    }
    return(TabClass::Scroll_Map(facing, distance, really));
}

void HelpClass::Set_Cost(int cost)
{
    Cost = cost;
}

void HelpClass::Set_Tactical_Position(COORDINATE coord)
{
    TacticalCoord = coord;
}
