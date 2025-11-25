#include "mmx_stub.h"
#include <cstdio> // For printf

// Defined in GLOBALS.cpp; referenced here to avoid duplicate definition.
extern bool MMXAvailable;

bool Detect_MMX_Availability() {
    printf("Detect_MMX_Availability: MMX detection stub called.\n");
    MMXAvailable = false; // Assume no MMX for now
    return MMXAvailable;
}

void Init_MMX() {
    printf("Init_MMX: MMX initialization stub called.\n");
}
