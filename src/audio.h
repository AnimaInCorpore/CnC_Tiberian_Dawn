#pragma once

#include "legacy_compat.h"

int Sound_Effect(VocType voc, VolType volume, int variation = 1, signed short panvalue = 0);
void Sound_Effect(VocType voc, COORDINATE coord = 0, int variation = 1);
void Speak(VoxType voice);
void Speak_AI(void);
void Stop_Speaking(void);
bool Is_Speaking(void);

