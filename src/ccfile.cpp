#include "ccfile.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>

static long MinLong(long a, long b) { return (a < b) ? a : b; }

CCFileClass::CCFileClass(char const* filename)
    : CDFileClass(),
      FromDisk(false),
      Pointer(NULL),
      Start(0),
      Position(0),
      Length(0) {
    Set_Name(filename);
}

CCFileClass::CCFileClass() : CDFileClass(), FromDisk(false), Pointer(NULL), Start(0), Position(0), Length(0) {}

void CCFileClass::Error(int, int, char const*) {
    // Early ports don't have the full UI/CDC verification flow; treat file errors as fatal.
    std::fprintf(stderr, "CCFileClass error opening/reading '%s'\n", File_Name() ? File_Name() : "<null>");
    std::fflush(stderr);
    std::exit(EXIT_FAILURE);
}

long CCFileClass::Write(void const* buffer, long size) {
    if (Pointer || FromDisk) {
        Error(EACCES, false, File_Name());
    }
    return CDFileClass::Write(buffer, size);
}

long CCFileClass::Read(void* buffer, long size) {
    int opened = false;

    if (!Is_Open()) {
        if (Open()) {
            opened = true;
        }
    }

    if (Pointer) {
        long maximum = Length - Position;
        size = MinLong(maximum, size);
        if (size > 0) {
            std::memcpy(buffer, static_cast<unsigned char const*>(Pointer) + Position, static_cast<size_t>(size));
            Position += size;
        }
        if (opened) Close();
        return size;
    }

    if (FromDisk) {
        long maximum = Length - Position;
        size = MinLong(maximum, size);
        if (size > 0) {
            CDFileClass::Seek(Start + Position, SEEK_SET);
            size = CDFileClass::Read(buffer, size);
            Position += size;
        }
        if (opened) Close();
        return size;
    }

    long read = CDFileClass::Read(buffer, size);
    if (opened) Close();
    return read;
}

long CCFileClass::Seek(long pos, int dir) {
    if (Pointer || FromDisk) {
        switch (dir) {
            case SEEK_END:
                Position = Length;
                break;
            case SEEK_SET:
                Position = 0;
                break;
            case SEEK_CUR:
            default:
                break;
        }
        Position += pos;
        if (Position < 0) Position = 0;
        if (Position > Length) Position = Length;
        return Position;
    }
    return CDFileClass::Seek(pos, dir);
}

long CCFileClass::Size() {
    if (Pointer || FromDisk) return Length;
    return CDFileClass::Size();
}

int CCFileClass::Is_Available(int) {
    if (MixFileClass::Offset(File_Name())) {
        return true;
    }
    return CDFileClass::Is_Available();
}

int CCFileClass::Is_Open() const {
    if (Pointer) return true;
    return CDFileClass::Is_Open();
}

void CCFileClass::Close() {
    FromDisk = false;
    Pointer = NULL;
    Position = 0;
    Start = 0;
    Length = 0;
    CDFileClass::Close();
}

int CCFileClass::Open(int rights) {
    Close();

    if ((rights & WRITE) || CDFileClass::Is_Available()) {
        return CDFileClass::Open(rights);
    }

    char const* mix_filename = NULL;
    if (MixFileClass::Offset(File_Name(), &Pointer, &mix_filename, &Start, &Length)) {
        if (!Pointer) {
            long start = Start;
            long length = Length;

            char const* duplicate = File_Name() ? ::strdup(File_Name()) : NULL;
            if (!mix_filename) {
                // No mixfile provided by the current shim; fall back to normal disk access.
                if (duplicate) std::free(const_cast<char*>(duplicate));
                return CDFileClass::Open(rights);
            }

            Open(mix_filename, READ);
            Searching(false);
            Set_Name(duplicate);
            Searching(true);
            if (duplicate) std::free(const_cast<char*>(duplicate));

            Start = start;
            Length = length;
            FromDisk = true;
        }
    } else {
        return CDFileClass::Open(rights);
    }

    return true;
}

// Backward compatibility helpers expected by some legacy code.
static CCFileClass Handles[10];

int __cdecl Open_File(char const* file_name, int mode) {
    for (int index = 0; index < static_cast<int>(sizeof(Handles) / sizeof(Handles[0])); ++index) {
        if (!Handles[index].Is_Open()) {
            Handles[index].Set_Name(file_name);
            if (Handles[index].Open(mode)) {
                return index;
            }
            break;
        }
    }
    return WW_ERROR;
}

VOID __cdecl Close_File(int handle) {
    if (handle != WW_ERROR && Handles[handle].Is_Open()) {
        Handles[handle].Close();
    }
}

LONG __cdecl Read_File(int handle, VOID* buf, ULONG bytes) {
    if (handle != WW_ERROR && Handles[handle].Is_Open()) {
        return Handles[handle].Read(buf, static_cast<long>(bytes));
    }
    return 0;
}

LONG __cdecl Write_File(int handle, VOID const* buf, ULONG bytes) {
    if (handle != WW_ERROR && Handles[handle].Is_Open()) {
        return Handles[handle].Write(buf, static_cast<long>(bytes));
    }
    return 0;
}

int __cdecl Find_File(char const* file_name) {
    CCFileClass file(file_name);
    return file.Is_Available();
}

VOID* __cdecl Load_Alloc_Data(char const* name, int) {
    CCFileClass file(name);
    return Load_Alloc_Data(static_cast<FileClass&>(file));
}

ULONG __cdecl File_Size(int handle) {
    if (handle != WW_ERROR && Handles[handle].Is_Open()) {
        return static_cast<ULONG>(Handles[handle].Size());
    }
    return 0;
}

ULONG __cdecl Seek_File(int handle, LONG offset, int starting) {
    if (handle != WW_ERROR && Handles[handle].Is_Open()) {
        return static_cast<ULONG>(Handles[handle].Seek(offset, starting));
    }
    return 0;
}

void WWDOS_Shutdown(void) {
    for (int index = 0; index < 10; ++index) {
        Handles[index].Set_Name(NULL);
    }
}

void Unfragment_File_Cache(void) {}
