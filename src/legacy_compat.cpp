#include "legacy_compat.h"

#include <cstdlib>
#include <cstdio>
#include <string>

void const* ObjectTypeClass::SelectShapes = NULL;
void const* ObjectTypeClass::PipShapes = NULL;

namespace {
std::FILE* Try_Open_File(const std::string& name) {
    if (name.empty()) return NULL;
    std::FILE* handle = std::fopen(name.c_str(), "rb");
    if (!handle && name.find('/') == std::string::npos) {
        std::string with_cd("CD/");
        with_cd += name;
        handle = std::fopen(with_cd.c_str(), "rb");
    }
    return handle;
}
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

void FileClass::Close() {
    if (Handle) {
        std::fclose(Handle);
        Handle = NULL;
    }
}

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

BuildingCollection Buildings;

TheaterDataType Theaters[THEATER_COUNT] = {
    {"DESERT", "DESERT", "DES"},
    {"JUNGLE", "JUNGLE", "JUN"},
    {"TEMPERATE", "TEMPERAT", "TEM"},
    {"WINTER", "WINTER", "WIN"},
};

TheaterType LastTheater = THEATER_NONE;

COORDINATE As_Coord(TARGET target) { return static_cast<COORDINATE>(target); }

BuildingClass* As_Building(TARGET) { return NULL; }

DirType Direction(COORDINATE coord1, COORDINATE coord2) {
    (void)coord1;
    (void)coord2;
    return FACING_NONE;
}

int Distance(COORDINATE coord1, COORDINATE coord2) { return std::abs(coord2 - coord1); }

int Fixed_To_Cardinal(int value, int percent_fixed) {
    return (value * percent_fixed) / 256;
}

int Get_Resolution_Factor(void) {
    return 0;
}

namespace MixFileClass {
void const* Retrieve(const char* filename) {
    (void)filename;
    return NULL;
}
}  // namespace MixFileClass
