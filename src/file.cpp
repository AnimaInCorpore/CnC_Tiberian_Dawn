#include "file.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>

namespace {
std::FILE* Try_Open_File_Mode(const std::string& name, const char* mode) {
    if (name.empty()) return NULL;
    std::FILE* handle = std::fopen(name.c_str(), mode);
    // Fallback: looked relative to CD/ if strictly reading (and no slash in name already).
    if (!handle && mode && mode[0] == 'r' && name.find('/') == std::string::npos) {
        std::string with_cd("CD/");
        with_cd += name;
        handle = std::fopen(with_cd.c_str(), mode);
    }
    return handle;
}

std::FILE* Try_Open_File(const std::string& name) { return Try_Open_File_Mode(name, "rb"); }
}  // namespace

FileClass::FileClass(const char* filename) : Filename(filename ? filename : ""), Handle(NULL) {}

FileClass::~FileClass() { Close(); }

bool FileClass::Is_Available() {
    if (Handle) return true;
    Handle = Try_Open_File(Filename);
    return Handle != NULL;
}

long FileClass::Size() {
    if (!Is_Available()) return 0;
    long old_pos = std::ftell(Handle);
    if (std::fseek(Handle, 0, SEEK_END) != 0) return 0;
    long size = std::ftell(Handle);
    std::fseek(Handle, old_pos, SEEK_SET);
    return size;
}

long FileClass::Read(void* buffer, long length) {
    if (!Is_Available() || !buffer || length <= 0) return 0;
    return static_cast<long>(std::fread(buffer, 1, static_cast<size_t>(length), Handle));
}

long FileClass::Write(void const* buffer, long length) {
    if (!buffer || length <= 0) return 0;
    if (!Handle) {
        Handle = Try_Open_File_Mode(Filename, "wb");
        if (!Handle) return 0;
    }
    return static_cast<long>(std::fwrite(buffer, 1, static_cast<size_t>(length), Handle));
}

void FileClass::Close() {
    if (Handle) {
        std::fclose(Handle);
        Handle = NULL;
    }
}

// Helpers

void* Load_Alloc_Data(FileClass& file) {
    if (!file.Is_Available()) return NULL;
    long size = file.Size();
    if (size <= 0) return NULL;

    unsigned char* buffer = new unsigned char[size];
    long read = file.Read(buffer, size);
    if (read <= 0) {
        delete[] buffer;
        return NULL;
    }
    return buffer;
}
