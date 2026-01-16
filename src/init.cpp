#include "function.h"

#include <cstring>

void Uninit_Game(void) {}

long Obfuscate(char const* string)
{
    if (!string) return 0;

    // Deterministic, non-cryptographic transform used only to keep legacy call
    // sites linkable during the incremental port.
    unsigned long hash = 2166136261UL;
    for (char const* p = string; *p; ++p) {
        hash ^= static_cast<unsigned char>(*p);
        hash *= 16777619UL;
    }
    return static_cast<long>(hash);
}

void Anim_Init(void) {}

bool Init_Game(int, char*[])
{
    // Portable build currently boots via `Main_Game` shim. Keep this function
    // as a no-op that succeeds so dependent modules can be ported safely.
    return true;
}

bool Select_Game(bool)
{
    // Menu system not ported yet.
    return false;
}

bool Parse_Command_Line(int, char*[])
{
    // Command line options not wired up in the portable shim yet.
    return true;
}

void Parse_INI_File(void) {}

int Version_Number(void)
{
    return 0;
}

void Save_Recording_Values(void) {}
void Load_Recording_Values(void) {}

