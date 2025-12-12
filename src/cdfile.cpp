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

	if (!g_search_head && RawPath[0] == '\0') {
		// Prefer explicit subfolder selection, then fall back to all known CD mirrors.
		if (!g_cd_subfolder.empty()) {
			const std::string primary = Join_Path("CD", g_cd_subfolder.c_str());
			Add_Search_Drive(const_cast<char*>(primary.c_str()));
		}
		// Always include the Tiberian Dawn unpacked discs first so fonts/MIXes are found.
		Add_Search_Drive(const_cast<char*>("CD/TIBERIAN_DAWN/CD1"));
		Add_Search_Drive(const_cast<char*>("CD/TIBERIAN_DAWN/CD2"));
		Add_Search_Drive(const_cast<char*>("CD/TIBERIAN_DAWN/CD3"));
		// Legacy mirrors.
		Add_Search_Drive(const_cast<char*>("CD/GDI"));
		Add_Search_Drive(const_cast<char*>("CD/NOD"));
		Add_Search_Drive(const_cast<char*>("CD/CNC95"));
		Add_Search_Drive(const_cast<char*>("CD"));
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
  while (cursor) {
    if (cursor->path == path) return;
    cursor = cursor->next;
  }

  auto* node = new SearchDriveNode();
  node->path = path;
  node->next = g_search_head;
  g_search_head = node;

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
