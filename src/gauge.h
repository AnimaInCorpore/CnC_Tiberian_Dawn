#pragma once

#include "control.h"

class GaugeClass : public ControlClass
{
public:
    GaugeClass(unsigned id, int x, int y, int w, int h);

    virtual int Draw_Me(int forced = false);
    virtual int Set_Maximum(int value);
    virtual int Set_Value(int value);
    virtual int Get_Value(void) const { return (CurValue); }
    virtual void Use_Thumb(int value) { HasThumb = value ? true : false; }

    virtual int Thumb_Pixels(void) { return (8); }

    unsigned IsColorized : 1;

protected:
    unsigned HasThumb : 1;
    unsigned IsHorizontal : 1;

    int MaxValue;
    int CurValue;
    int ClickDiff;

protected:
    virtual void Draw_Thumb(void);
    virtual int Action(unsigned flags, KeyNumType& key);
    virtual int Pixel_To_Value(int pixel);
    virtual int Value_To_Pixel(int value);
};

class TriColorGaugeClass : public GaugeClass
{
public:
    TriColorGaugeClass(unsigned id, int x, int y, int w, int h);
    virtual int Draw_Me(int forced);
    virtual int Set_Red_Limit(int value);
    virtual int Set_Yellow_Limit(int value);

protected:
    int RedLimit;
    int YellowLimit;
};

