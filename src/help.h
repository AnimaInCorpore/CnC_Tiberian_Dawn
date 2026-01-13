#pragma once

#include "tab.h"
#include "ftimer.h"

class HelpClass : public TabClass {
public:
    HelpClass(void);
    virtual ~HelpClass(void) {}

    virtual void Init_Clear(void);
    virtual void Draw_It(bool complete = false);
    virtual void AI(KeyNumType& input, int x, int y);
    virtual bool Scroll_Map(DirType facing, int& distance, bool really);
    virtual void Set_Tactical_Position(COORDINATE coord);

    void Help_Text(int text, int x = -1, int y = -1, int color = LTGREY, bool quick = false, int cost = 0);
    void Set_Cost(int cost);
    short const* Overlap_List(void) const;

    virtual void Code_Pointers(void);
    virtual void Decode_Pointers(void);

private:
    static char const* HelpText;
    static CountDownTimerClass CountDownTimer;
    
    void Set_Text(int text);

    unsigned IsRight : 1;
    int Cost;
    int X;
    int Y;

    int DrawX;
    int DrawY;
    int Width;
    int Text;
    int Color;

    static short const OverlapList[30];

    enum HelpClassEnum {
        HELP_DELAY = TICKS_PER_SECOND * 1,
        Y_OFFSET = 0,
        X_OFFSET = 10
    };
};
