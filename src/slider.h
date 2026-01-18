#pragma once

#include "gauge.h"
#include "shapebtn.h"

class SliderClass : public GaugeClass {
public:
    SliderClass(unsigned id, int x, int y, int w, int h, int belong_to_list = false);
    virtual ~SliderClass(void);

    virtual void Set_Thumb_Size(int value);
    virtual int Set_Maximum(int value);
    virtual int Set_Value(int value);
    virtual int Bump(int up);
    virtual int Step(int up);
    virtual int Draw_Me(int forced = false);
    virtual void Peer_To_Peer(unsigned flags, KeyNumType& key, ControlClass& whom);

    virtual int Thumb_Pixels(void) { return ThumbSize; }

protected:
    ShapeButtonClass* PlusGadget;
    ShapeButtonClass* MinusGadget;

    unsigned BelongToList : 1;
    int Thumb;
    int ThumbSize;
    int ThumbStart;

    virtual int Action(unsigned flags, KeyNumType& key);
    virtual void Draw_Thumb(void);

private:
    void Recalc_Thumb(void);
};

