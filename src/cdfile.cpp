#include "legacy/cdfile.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <string>
#include <vector>

namespace {

struct SearchDriveNode {
  SearchDriveNode* next = nullptr;
  std::string path;
};

SearchDriveNode* g_search_head = nullptr;
std::string g_cd_subfolder;
std::string g_data_root;

void Clear_Search_List() {
  while (g_search_head) {
    SearchDriveNode* next = g_search_head->next;
    delete g_search_head;
    g_search_head = next;
  }
}

std::string Join_Path(std::string const& base, char const* filename) {
  if (base.empty()) return filename ? filename : std::string{};
  if (!filename || !*filename) return base;
  if (base.back() == '/' || base.back() == '\\') {
    return base + filename;
  }
  return base + "/" + filename;
}

std::string Trim_Spaces(const char* value) {
  if (!value) return {};
  const char* start = value;
  while (*start && std::isspace(static_cast<unsigned char>(*start))) ++start;
  const char* end = start + std::strlen(start);
  while (end > start && std::isspace(static_cast<unsigned char>(end[-1]))) --end;
  return std::string(start, static_cast<std::size_t>(end - start));
}

std::string Normalize_Data_Root(const char* root) {
  std::string trimmed = Trim_Spaces(root);
  while (!trimmed.empty() && (trimmed.back() == '/' || trimmed.back() == '\\')) {
    trimmed.pop_back();
  }
  return trimmed;
}

std::string Normalize_Cd_Subfolder(const char* subfolder) {
  if (!subfolder || !*subfolder) return {};
  std::string value(subfolder);
  std::transform(value.begin(), value.end(), value.begin(),
                 [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
  if (value == "GDI" || value == "NOD") {
    return value;
  }
  return {};
}

}  // namespace

CDFileClass::SearchDriveType* CDFileClass::First = nullptr;
char CDFileClass::RawPath[512] = {};
int CDFileClass::CurrentCDDrive = 0;
int CDFileClass::LastCDDrive = 0;

CDFileClass::CDFileClass(char const* filename) : RawFileClass(filename), IsDisabled(0) {}

CDFileClass::CDFileClass() : RawFileClass(), IsDisabled(0) {}

void CDFileClass::Set_Data_Root(char const* root) {
  const std::string normalized = Normalize_Data_Root(root);
  if (normalized == g_data_root) return;
  g_data_root = normalized;
  Clear_Search_Drives();
}

char const* CDFileClass::Get_Data_Root(void) {
  return g_data_root.empty() ? nullptr : g_data_root.c_str();
}

void CDFileClass::Set_CD_Subfolder(char const* subfolder) {
  const std::string normalized = Normalize_Cd_Subfolder(subfolder);
  if (normalized == g_cd_subfolder) {
    return;
  }
  g_cd_subfolder = normalized;
  Clear_Search_Drives();
}

char const* CDFileClass::Get_CD_Subfolder(void) {
  return g_cd_subfolder.empty() ? nullptr : g_cd_subfolder.c_str();
}

char const* CDFileClass::Set_Name(char const* filename) {
  return RawFileClass::Set_Name(filename);
}

int CDFileClass::Open(char const* filename, int rights) {
  Set_Name(filename);
  return Open(rights);
}

int CDFileClass::Open(int rights) {
  if ((rights & WRITE) != 0) {
    return RawFileClass::Open(rights);
  }

  const std::string original_name = File_Name() ? File_Name() : "";

  if (!g_search_head && RawPath[0] == '\0') {
    auto add_default_paths = [&](const std::string& root) {
      auto add_drive = [&](const std::string& path) {
        if (path.empty()) return;
        Add_Search_Drive(const_cast<char*>(path.c_str()));
      };

      std::string cd_root;
      if (root.empty()) {
        cd_root = "CD";
      } else {
        const auto tail = root.find_last_of("/\\");
        const std::string leaf = tail == std::string::npos ? root : root.substr(tail + 1);
        std::string upper = leaf;
        std::transform(upper.begin(), upper.end(), upper.begin(),
                       [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
        cd_root = (upper == "CD") ? root : Join_Path(root, "CD");
      }

      // Allow assets to live directly under the data root (e.g. mixfiles unpacked flat).
      if (!root.empty()) {
        add_drive(root);
      }

      // Prefer explicit subfolder selection, then fall back to all known CD mirrors.
      if (!g_cd_subfolder.empty()) {
        add_drive(Join_Path(cd_root, g_cd_subfolder.c_str()));
      }

      static const char* kDiscs[] = {"CD1", "CD2", "CD3"};
      for (auto disc : kDiscs) {
        add_drive(Join_Path(Join_Path(cd_root, "TIBERIAN_DAWN"), disc));
      }

      add_drive(Join_Path(cd_root, "GDI"));
      add_drive(Join_Path(cd_root, "NOD"));
      add_drive(Join_Path(cd_root, "CNC95"));
      add_drive(cd_root);
    };

    add_default_paths(g_data_root);
  }

  if (!IsDisabled && First) {
    SearchDriveNode* node = g_search_head;
    while (node) {
      const std::string candidate = Join_Path(node->path, File_Name());
      RawFileClass::Set_Name(candidate.c_str());
      if (RawFileClass::Open(rights)) {
        return 1;
      }
      node = node->next;
    }
  }

  if (!original_name.empty()) {
    RawFileClass::Set_Name(original_name.c_str());
  }
  return RawFileClass::Open(rights);
}

int CDFileClass::Set_Search_Drives(char* pathlist) {
  Clear_Search_Drives();
  if (!pathlist) return 0;

  std::strncpy(RawPath, pathlist, sizeof(RawPath) - 1);
  RawPath[sizeof(RawPath) - 1] = '\0';

  int count = 0;
  char* token = std::strtok(pathlist, ";");
  while (token) {
    Add_Search_Drive(token);
    ++count;
    token = std::strtok(nullptr, ";");
  }
  return count;
}

void CDFileClass::Add_Search_Drive(char* path) {
  if (!path || !*path) return;

  SearchDriveNode* cursor = g_search_head;
  SearchDriveNode* tail = nullptr;
  while (cursor) {
    if (cursor->path == path) return;
    tail = cursor;
    cursor = cursor->next;
  }

  auto* node = new SearchDriveNode();
  node->path = path;
  if (!g_search_head) {
    g_search_head = node;
  } else if (tail) {
    tail->next = node;
  }

  First = reinterpret_cast<SearchDriveType*>(g_search_head);
}

void CDFileClass::Clear_Search_Drives(void) {
  Clear_Search_List();
  First = nullptr;
  RawPath[0] = '\0';
}

void CDFileClass::Refresh_Search_Drives(void) { Clear_Search_Drives(); }

void CDFileClass::Set_CD_Drive(int drive) {
  CurrentCDDrive = drive;
  LastCDDrive = drive;
}
