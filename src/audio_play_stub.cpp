// Minimal playback stubs to satisfy references while the full audio decoder/mixer
// is implemented. These intentionally do not produce sound yet.

#include "function.h"

int Play_Sample(void const * /*data*/, int /*priority*/, int /*volume*/, int /*pan*/) {
    // TODO: implement decoding/mixing of .AUD/.JUV/.Vxx assets via SDL audio.
    return -1;
}

int Play_Sample(void const *data, int priority, int volume) {
    return Play_Sample(data, priority, volume, 0);
}

bool Is_Sample_Playing(void const * /*data*/) {
    return false;
}

void Stop_Sample_Playing(void const * /*data*/) {
}
