#pragma once

class CrewClass {
public:
    unsigned short Kills;

    CrewClass(void) : Kills(0) {}

    int Made_A_Kill(void) {
        ++Kills;
        return Kills;
    }
};

