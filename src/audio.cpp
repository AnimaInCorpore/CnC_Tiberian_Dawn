#include "audio.h"

int Sound_Effect(VocType, VolType, int, signed short) {
    return -1;
}

void Sound_Effect(VocType voc, COORDINATE, int variation) {
    (void)Sound_Effect(voc, 0, variation, 0);
}

void Speak(VoxType voice) { SpeakQueue = voice; }

void Speak_AI(void) { SpeakQueue = VOX_NONE; }

void Stop_Speaking(void) { SpeakQueue = VOX_NONE; }

bool Is_Speaking(void) {
    Speak_AI();
    return false;
}

