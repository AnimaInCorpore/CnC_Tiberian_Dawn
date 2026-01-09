#include "cdfile.h"

#include <cctype>
#include <cstring>
#include <string>
#include <vector>

#include <sys/stat.h>

namespace {
static std::vector<std::string> SearchPaths;
static std::string RawPath;
static int CurrentCDDrive = 0;
static int LastCDDrive = 0;

static bool Is_Dir(const std::string& path) {
    struct stat st;
    if (path.empty()) return false;
    if (stat(path.c_str(), &st) != 0) return false;
    return (st.st_mode & S_IFDIR) != 0;
}

static std::string Normalize_Path(const char* input) {
    if (!input) return std::string();
    std::string out(input);
    for (std::string::iterator it = out.begin(); it != out.end(); ++it) {
        if (*it == '\\') *it = '/';
    }
    return out;
}

static bool Has_Drive_Letter(const std::string& path) {
    return path.size() >= 2 && std::isalpha(static_cast<unsigned char>(path[0])) != 0 && path[1] == ':';
}

static std::string Strip_Leading_Slashes(std::string path) {
    while (!path.empty() && (path[0] == '/' || path[0] == '\\')) path.erase(0, 1);
    return path;
}

static std::string Ensure_Trailing_Slash(std::string path) {
    if (!path.empty() && path[path.size() - 1] != '/') path += '/';
    return path;
}

static std::string Normalize_Search_Entry(const char* raw_entry) {
    std::string entry = Normalize_Path(raw_entry);
    if (entry.empty()) return entry;

    if (entry.size() >= 2 && entry[0] == '?' && entry[1] == ':') {
        entry.erase(0, 2);
        entry = Strip_Leading_Slashes(entry);
        entry = std::string("CD/") + entry;
        return Ensure_Trailing_Slash(entry);
    }

    if (!entry.empty() && entry[0] == '/') {
        entry = Strip_Leading_Slashes(entry);
        entry = std::string("CD/") + entry;
        return Ensure_Trailing_Slash(entry);
    }

    return Ensure_Trailing_Slash(entry);
}

static void Ensure_Default_Search_Paths() {
    if (!SearchPaths.empty()) return;

    const char* candidates[] = {
        "CD/TIBERIAN_DAWN/CD2/",
        "CD/TIBERIAN_DAWN/CD1/",
        "CD/TIBERIAN_DAWN/CD3/",
        "CD/TIBERIAN_DAWN/",
        "CD/",
    };

    for (size_t i = 0; i < sizeof(candidates) / sizeof(candidates[0]); ++i) {
        std::string path(candidates[i]);
        if (!Is_Dir(path)) continue;
        SearchPaths.push_back(path);
    }
}

static bool Has_Path_Separator(const std::string& path) {
    return path.find('/') != std::string::npos || path.find('\\') != std::string::npos;
}

static std::string Strip_Drive_Prefix(std::string path) {
    if (!Has_Drive_Letter(path)) return path;
    path.erase(0, 2);
    return path;
}

static bool Try_Open_Raw(RawFileClass& file, const std::string& candidate, int rights) {
    file.Close();
    file.RawFileClass::Set_Name(candidate.c_str());
    return file.RawFileClass::Open(rights) != 0;
}
}  // namespace

CDFileClass::CDFileClass(char const* filename) : RawFileClass(filename), IsDisabled(false) {
    if (filename) Set_Name(filename);
}

CDFileClass::CDFileClass() : RawFileClass(), IsDisabled(false) {}

bool CDFileClass::Is_There_Search_Drives() {
    Ensure_Default_Search_Paths();
    return !SearchPaths.empty();
}

int CDFileClass::Set_Search_Drives(char* pathlist) {
    if (!pathlist || !*pathlist) return 0;

    Ensure_Default_Search_Paths();

    RawPath = pathlist;

    const char* cursor = pathlist;
    while (*cursor) {
        while (*cursor == ';') ++cursor;
        const char* segment = cursor;
        while (*cursor && *cursor != ';') ++cursor;
        std::string token(segment, cursor - segment);
        if (!token.empty()) {
            std::string normalized = Normalize_Search_Entry(token.c_str());
            if (!normalized.empty()) SearchPaths.push_back(normalized);
        }
        if (*cursor == ';') ++cursor;
    }

    return 0;
}

void CDFileClass::Add_Search_Drive(char* path) {
    if (!path) return;
    Ensure_Default_Search_Paths();
    std::string normalized = Normalize_Search_Entry(path);
    if (normalized.empty()) return;
    SearchPaths.push_back(normalized);
}

void CDFileClass::Clear_Search_Drives() {
    SearchPaths.clear();
}

void CDFileClass::Refresh_Search_Drives() {
    Clear_Search_Drives();
    Ensure_Default_Search_Paths();

    if (RawPath.empty()) return;

    std::string raw_copy = RawPath;
    std::vector<char> buffer(raw_copy.begin(), raw_copy.end());
    buffer.push_back('\0');
    Set_Search_Drives(&buffer[0]);
}

void CDFileClass::Set_CD_Drive(int drive) {
    LastCDDrive = CurrentCDDrive;
    CurrentCDDrive = drive;
}

int CDFileClass::Get_CD_Drive() { return CurrentCDDrive; }

int CDFileClass::Get_Last_CD_Drive() { return LastCDDrive; }

char const* CDFileClass::Set_Name(char const* filename) {
    RawFileClass::Set_Name(filename);

    if (!filename || !*filename) return File_Name();
    if (IsDisabled) return File_Name();

    Ensure_Default_Search_Paths();

    if (RawFileClass::Is_Available()) return File_Name();

    std::string name = Normalize_Path(filename);
    name = Strip_Drive_Prefix(name);
    name = Strip_Leading_Slashes(name);

    for (size_t i = 0; i < SearchPaths.size(); ++i) {
        const std::string& path = SearchPaths[i];
        std::string candidate = path + name;
        RawFileClass::Set_Name(candidate.c_str());
        if (RawFileClass::Is_Available()) return File_Name();
    }

    RawFileClass::Set_Name(filename);
    return File_Name();
}

int CDFileClass::Open(char const* filename, int rights) {
    Close();
    if (!filename) {
        return 0;
    }
    if (IsDisabled || (rights & WRITE)) {
        return RawFileClass::Open(filename, rights);
    }
    Set_Name(filename);
    return RawFileClass::Open(rights);
}

int CDFileClass::Open(int rights) {
    if (IsDisabled || (rights & WRITE)) return RawFileClass::Open(rights);

    Ensure_Default_Search_Paths();

    const char* current = File_Name();
    if (!current || !*current) return RawFileClass::Open(rights);
    std::string original_name(current);

    std::string name = Normalize_Path(original_name.c_str());
    name = Strip_Drive_Prefix(name);

    if (Try_Open_Raw(*this, name, rights)) return 1;

    if (Has_Path_Separator(name) && name.compare(0, 3, "CD/") != 0) {
        if (Try_Open_Raw(*this, std::string("CD/") + Strip_Leading_Slashes(name), rights)) return 1;
    }

    std::string leaf = Strip_Leading_Slashes(name);
    for (size_t i = 0; i < SearchPaths.size(); ++i) {
        const std::string& path = SearchPaths[i];
        if (Try_Open_Raw(*this, path + leaf, rights)) return 1;
    }

    RawFileClass::Set_Name(original_name.c_str());
    return RawFileClass::Open(rights);
}
