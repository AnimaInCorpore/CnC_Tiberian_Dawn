#include "port_paths.h"

#include <SDL.h>

#include <filesystem>

namespace {
std::string TD_Pref_Dir() {
  char* pref = SDL_GetPrefPath("CnC", "TiberianDawn");
  if (!pref) {
    return {};
  }
  std::string result(pref);
  SDL_free(pref);
  return result;
}

std::string TD_Pref_File(const char* basename) {
  if (!basename || !*basename) {
    return {};
  }
  std::string dir = TD_Pref_Dir();
  if (dir.empty()) {
    return std::string(basename);
  }
  return dir + basename;
}
}  // namespace

std::string TD_Resolve_Profile_Read(const char* basename) {
  if (!basename || !*basename) {
    return {};
  }

  try {
    if (std::filesystem::exists(basename)) {
      return std::string(basename);
    }
  } catch (...) {
  }

  const std::string pref = TD_Pref_File(basename);
  try {
    if (!pref.empty() && std::filesystem::exists(pref)) {
      return pref;
    }
  } catch (...) {
  }

  return std::string(basename);
}

std::string TD_Resolve_Profile_Write(const char* basename) {
  if (!basename || !*basename) {
    return {};
  }

  try {
    if (std::filesystem::exists(basename)) {
      return std::string(basename);
    }
  } catch (...) {
  }

  return TD_Pref_File(basename);
}

