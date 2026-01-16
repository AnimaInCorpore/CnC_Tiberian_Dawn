#include "rawfile.h"

#include <cstdio>

char const* RawFileClass::Set_Name(char const* filename) {
    Filename = filename ? filename : "";
    Close();
    return Filename.empty() ? NULL : Filename.c_str();
}

char const* RawFileClass::File_Name() const {
    return Filename.empty() ? NULL : Filename.c_str();
}

int RawFileClass::Open(int rights) {
    if (rights & WRITE) {
        Close();
        if (Filename.empty()) return false;
        Handle = std::fopen(Filename.c_str(), "wb");
        return Handle != NULL;
    }
    return FileClass::Is_Available() ? true : false;
}

int RawFileClass::Open(char const* filename, int rights) {
    Set_Name(filename);
    return Open(rights);
}

int RawFileClass::Is_Open() const {
    return Handle != NULL;
}

int RawFileClass::Is_Available(int forced) {
    (void)forced;
    return FileClass::Is_Available() ? true : false;
}

long RawFileClass::Seek(long pos, int dir) {
    if (!Is_Open() && !Open(READ)) return 0;
    if (!Handle) return 0;
    if (std::fseek(Handle, pos, dir) != 0) return 0;
    return std::ftell(Handle);
}

