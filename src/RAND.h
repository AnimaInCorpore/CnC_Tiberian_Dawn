#pragma once

#include <cstdint>

extern int SimRandIndex;

int Sim_Random();
int Sim_IRandom(int minval, int maxval);
void Set_Sim_Random_Index(std::uint8_t index);
