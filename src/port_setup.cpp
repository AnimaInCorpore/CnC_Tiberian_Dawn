#include "port_setup.h"

#include <cstring>
#include <string>

#include "legacy/rawfile.h"
#include "port_paths.h"

namespace {
constexpr char kConquerIniName[] = "CONQUER.INI";

// Keep this intentionally minimal: enough for legacy startup paths that insist
// on CONQUER.INI existence, while the real INI parsing/writing is still being
// ported.
constexpr char kDefaultConquerIni[] =
    "[Options]\n"
    "GameSpeed=4\n"
    "ScrollRate=4\n"
    "Brightness=128\n"
    "Volume=160\n"
    "ScoreVolume=255\n"
    "Contrast=128\n"
    "Color=128\n"
    "Tint=128\n"
    "AutoScroll=1\n"
    "IsScoreRepeat=0\n"
    "IsScoreShuffle=0\n"
    "DeathAnnounce=0\n"
    "FreeScrolling=0\n"
    "SlowPalette=1\n"
    "\n"
    "[Intro]\n"
    "PlayIntro=Yes\n";
}  // namespace

bool Ensure_Default_Conquer_Ini() {
  if (RawFileClass(kConquerIniName).Is_Available()) {
    return true;
  }

  const std::string path = TD_Resolve_Profile_Write(kConquerIniName);
  RawFileClass ini(path.c_str());
  if (ini.Is_Available()) {
    return true;
  }

  if (!ini.Create()) {
    return false;
  }

  const long written =
      ini.Write(kDefaultConquerIni, static_cast<long>(std::strlen(kDefaultConquerIni)));
  ini.Close();
  return written > 0;
}
