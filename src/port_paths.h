#pragma once

#include <string>

// Resolves read/write locations for legacy profile/config files (e. g. CONQUER.INI).
// Reads prefer the working directory when present; writes go to the working directory only
// when the file already exists there, otherwise they go to the per-user SDL pref path.
std::string TD_Resolve_Profile_Read(const char* basename);
std::string TD_Resolve_Profile_Write(const char* basename);

