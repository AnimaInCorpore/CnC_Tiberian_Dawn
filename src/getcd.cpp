#include "legacy/getcd.h"

#include <filesystem>

namespace {

bool Looks_Like_Cnc_Disc_Root(const std::filesystem::path& root) {
  try {
    if (!std::filesystem::exists(root) || !std::filesystem::is_directory(root)) {
      return false;
    }
    // Any of these indicates a usable C&C disc mirror root.
    if (std::filesystem::exists(root / "GENERAL.MIX")) return true;
    if (std::filesystem::exists(root / "CONQUER.MIX")) return true;
    if (std::filesystem::exists(root / "MOVIES.MIX")) return true;
  } catch (...) {
  }
  return false;
}

}  // namespace

GetCDClass::GetCDClass() : DriveCount(0), Cursor(-1), Drives{} {}

void GetCDClass::Refresh() {
  const int previous_cursor = Cursor;
  DriveCount = 0;

  auto add_drive = [&](int id, const std::filesystem::path& root) {
    if (DriveCount >= static_cast<int>(sizeof(Drives) / sizeof(Drives[0]))) return;
    if (!Looks_Like_Cnc_Disc_Root(root)) return;
    for (int i = 0; i < DriveCount; ++i) {
      if (Drives[i] == id) return;
    }
    Drives[DriveCount++] = id;
  };

  // Canonical repo-local mirrors (C&C discs extracted to folders).
  add_drive(1, std::filesystem::path("CD") / "TIBERIAN_DAWN" / "CD1");
  add_drive(2, std::filesystem::path("CD") / "TIBERIAN_DAWN" / "CD2");
  add_drive(3, std::filesystem::path("CD") / "TIBERIAN_DAWN" / "CD3");

  // Legacy mirrors used by some local installs.
  add_drive(10, std::filesystem::path("CD") / "GDI");
  add_drive(11, std::filesystem::path("CD") / "NOD");
  add_drive(12, std::filesystem::path("CD") / "COVERT");
  add_drive(13, std::filesystem::path("CD") / "CNC95");
  add_drive(14, std::filesystem::path("CD"));

  Cursor = previous_cursor;
  if (Cursor < -1) {
    Cursor = -1;
  } else if (Cursor >= DriveCount) {
    Cursor = DriveCount > 0 ? (DriveCount - 1) : -1;
  }
}

int GetCDClass::Get_First_CD_Drive() {
  Refresh();
  if (DriveCount <= 0) return 0;
  Cursor = 0;
  return Drives[0];
}

int GetCDClass::Get_Number_Of_Drives() {
  Refresh();
  Cursor = -1;
  return DriveCount;
}

int GetCDClass::Get_Next_CD_Drive() {
  Refresh();
  if (DriveCount <= 0) return 0;
  ++Cursor;
  if (Cursor < 0 || Cursor >= DriveCount) {
    Cursor = 0;
  }
  return Drives[Cursor];
}
