#include "legacy_compat.h"

#include <cstdlib>
#include <cstdio>
#include <string>

void const* ObjectTypeClass::SelectShapes = NULL;
void const* ObjectTypeClass::PipShapes = NULL;

VoxType SpeakQueue = VOX_NONE;

namespace {
std::FILE* Try_Open_File_Mode(const std::string& name, const char* mode) {
    if (name.empty()) return NULL;
    std::FILE* handle = std::fopen(name.c_str(), mode);
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
MapClass Map;

namespace {
struct IniCursor {
    char const* Ptr;
    explicit IniCursor(char const* ptr) : Ptr(ptr ? ptr : "") {}
};

static void Skip_Spaces(IniCursor& cursor) {
    while (*cursor.Ptr == ' ' || *cursor.Ptr == '\t' || *cursor.Ptr == '\r') ++cursor.Ptr;
}

static bool Read_Line(IniCursor& cursor, std::string& line) {
    line.clear();
    if (!cursor.Ptr || !*cursor.Ptr) return false;
    while (*cursor.Ptr && *cursor.Ptr != '\n') {
        line.push_back(*cursor.Ptr);
        ++cursor.Ptr;
    }
    if (*cursor.Ptr == '\n') ++cursor.Ptr;
    return true;
}

static bool Parse_Section(std::string const& line, std::string& out_section) {
    if (line.size() < 2) return false;
    if (line[0] != '[') return false;
    size_t close = line.find(']');
    if (close == std::string::npos || close <= 1) return false;
    out_section = line.substr(1, close - 1);
    return true;
}

static bool Split_Key_Value(std::string const& line, std::string& out_key, std::string& out_value) {
    size_t eq = line.find('=');
    if (eq == std::string::npos) return false;
    out_key = line.substr(0, eq);
    out_value = line.substr(eq + 1);
    return true;
}

static void Trim(std::string& value) {
    while (!value.empty() && (value[0] == ' ' || value[0] == '\t')) value.erase(0, 1);
    while (!value.empty() && (value[value.size() - 1] == ' ' || value[value.size() - 1] == '\t' ||
                              value[value.size() - 1] == '\r')) {
        value.erase(value.size() - 1);
    }
}

static bool Equals_CI(std::string const& a, char const* b) {
    if (!b) return a.empty();
    return strcasecmp(a.c_str(), b) == 0;
}

static bool Find_Ini_Value(char const* section, char const* key, char const* buffer, std::string& out_value) {
    if (!buffer || !section || !key) return false;
    IniCursor cursor(buffer);
    std::string line;
    std::string current_section;
    bool in_section = false;

    while (Read_Line(cursor, line)) {
        if (!line.empty() && (line[0] == ';' || line[0] == '#')) continue;

        std::string found_section;
        if (Parse_Section(line, found_section)) {
            Trim(found_section);
            current_section = found_section;
            in_section = Equals_CI(current_section, section);
            continue;
        }

        if (!in_section) continue;

        std::string parsed_key;
        std::string parsed_value;
        if (!Split_Key_Value(line, parsed_key, parsed_value)) continue;
        Trim(parsed_key);
        Trim(parsed_value);
        if (!Equals_CI(parsed_key, key)) continue;

        out_value = parsed_value;
        return true;
    }
    return false;
}
}  // namespace

int WWGetPrivateProfileInt(char const* section, char const* key, int default_value, char const* buffer) {
    std::string value;
    if (!Find_Ini_Value(section, key, buffer, value)) return default_value;
    return std::atoi(value.c_str());
}

int WWGetPrivateProfileString(char const* section,
                              char const* key,
                              char const* default_value,
                              char* output,
                              int output_len,
                              char const* buffer) {
    if (!output || output_len <= 0) return 0;

    std::string value;
    if (!Find_Ini_Value(section, key, buffer, value)) {
        value = default_value ? default_value : "";
    }

    std::snprintf(output, static_cast<size_t>(output_len), "%s", value.c_str());
    output[output_len - 1] = '\0';
    return static_cast<int>(std::strlen(output));
}

bool WWWritePrivateProfileString(char const*, char const*, char const*, char*) { return true; }
bool WWWritePrivateProfileInt(char const*, char const*, int, char*) { return true; }

StructType BuildingTypeClass::From_Name(char const* name) {
    if (!name) return STRUCT_NONE;
    // Placeholder: real mapping will come from the building type database.
    (void)name;
    return STRUCT_NONE;
}

BuildingTypeClass const& BuildingTypeClass::As_Reference(StructType type) {
    static BuildingTypeClass refs[STRUCT_COUNT + 1];
    static bool initialized = false;
    static BuildingTypeClass none;

    if (!initialized) {
        for (int i = 0; i < STRUCT_COUNT + 1; ++i) {
            refs[i] = BuildingTypeClass();
            refs[i].IniName[0] = '\0';
            refs[i].Type = static_cast<StructType>(i);
        }
        initialized = true;
    }

    if (type < STRUCT_FIRST || type >= STRUCT_COUNT) return none;
    return refs[static_cast<int>(type)];
}

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
