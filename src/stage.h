#pragma once

class StageClass {
    unsigned short Stage;
    unsigned char StageTimer;
    unsigned char Rate;

public:
    StageClass() : Stage(0), StageTimer(0), Rate(0) {}

    int Fetch_Stage(void) const { return Stage; }
    int Fetch_Rate(void) const { return Rate; }
    void Set_Stage(int stage) { Stage = static_cast<unsigned short>(stage); }
    void Set_Rate(unsigned char rate) {
        Rate = rate;
        StageTimer = rate;
    }
    void AI(void) {}

    bool Graphic_Logic(void) {
        if (Rate) {
            --StageTimer;
            if (!StageTimer) {
                ++Stage;
                StageTimer = Rate;
                return true;
            }
        }
        return false;
    }

    void Code_Pointers(void) { return; }
    void Decode_Pointers(void) { return; }
};
