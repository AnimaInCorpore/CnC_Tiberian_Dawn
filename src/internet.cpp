/*
**  Command & Conquer(tm)
**  Copyright 2025 Electronic Arts Inc.
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// Portable port of INTERNET.CPP (Planet Westwood / WChat glue).

#include "legacy/ccdde.h"
#include "legacy/ccfile.h"
#include "legacy/externs.h"
#include "legacy/function.h"
#include "legacy/rawfile.h"
#include "legacy/tcpip.h"

#include "port_paths.h"

#include <cstdlib>
#include <cstring>
#include <string>

namespace {

bool Read_File_To_Buffer(const char* filename, std::string& out) {
  if (!filename || !*filename) return false;
  RawFileClass file(filename);
  if (!file.Is_Available()) return false;
  const long size = file.Size();
  if (size <= 0) return false;
  out.resize(static_cast<std::size_t>(size));
  if (file.Read(out.data(), size) != size) return false;
  return true;
}

bool Read_Profile_String(const char* section, const char* key, const char* def, char* out, int out_len) {
  if (!out || out_len <= 0) return false;
  out[0] = '\0';

  const std::string path = TD_Resolve_Profile_Read("CONQUER.INI");
  std::string ini;
  if (!Read_File_To_Buffer(path.c_str(), ini)) {
    std::strncpy(out, def ? def : "", static_cast<std::size_t>(out_len - 1));
    out[out_len - 1] = '\0';
    return false;
  }

  ini.push_back('\0');
  WWGetPrivateProfileString(section, key, def ? def : "", out, out_len, ini.data());
  return true;
}

bool Launch_Command_If_Configured(const char* env_var) {
  const char* cmd = std::getenv(env_var);
  if (!cmd || !*cmd) return false;
  const int rc = std::system(cmd);
  return rc == 0;
}

}  // namespace

char PlanetWestwoodHandle[] = {"Handle"};
char PlanetWestwoodPassword[] = {"Password"};
unsigned long PlanetWestwoodGameID = 0;
unsigned long PlanetWestwoodStartTime = 0;
HWND WChatHWND = nullptr;
int WChatMaxAhead = 0;
int WChatSendRate = 0;

extern bool SpawnedFromWChat;

void Check_From_WChat(char* wchat_name) {
#ifndef DEMO
  const char default_string[] = {"Error"};
  char key_string[256]{};
  const char* ini_buffer = nullptr;
  std::string owned;

  if (wchat_name) {
    if (!Read_File_To_Buffer(wchat_name, owned)) {
      return;
    }
    owned.push_back('\0');
    ini_buffer = owned.data();
  } else {
    ini_buffer = DDEServer.Get_MPlayer_Game_Info();
  }

  if (!ini_buffer) return;

  WWGetPrivateProfileString("Internet", "Address", default_string, key_string, sizeof(key_string),
                            const_cast<char*>(ini_buffer));
  if (!std::strcmp(key_string, default_string)) return;
  std::strncpy(PlanetWestwoodIPAddress, key_string, sizeof(PlanetWestwoodIPAddress) - 1);
  PlanetWestwoodIPAddress[sizeof(PlanetWestwoodIPAddress) - 1] = '\0';

  WWGetPrivateProfileString("Internet", "Port", default_string, key_string, sizeof(key_string),
                            const_cast<char*>(ini_buffer));
  if (!std::strcmp(key_string, default_string)) return;
  PlanetWestwoodPortNumber = std::atol(key_string);

  WWGetPrivateProfileString("Internet", "Host", default_string, key_string, sizeof(key_string),
                            const_cast<char*>(ini_buffer));
  if (!std::strcmp(key_string, default_string)) return;
  PlanetWestwoodIsHost = (std::strchr(key_string, '1') != nullptr);
#endif
}

int Read_Game_Options(char* name) {
  char filename[256] = {"INVALID.123"};
  if (name) {
    std::strncpy(filename, name, sizeof(filename) - 1);
    filename[sizeof(filename) - 1] = '\0';
  }

  std::string owned;
  char* buffer = nullptr;

  CCFileClass file(filename);
  if (name && !file.Is_Available()) {
    return 0;
  }

  if (name) {
    owned.assign(8192, '\0');
    const long bytes = file.Read(owned.data(), static_cast<long>(owned.size() - 1));
    file.Close();
    if (bytes <= 0) return 0;
    buffer = owned.data();
  } else {
    buffer = DDEServer.Get_MPlayer_Game_Info();
  }

  if (!buffer) return 0;

  WWGetPrivateProfileString("Options", "Handle", "Noname", MPlayerName, sizeof(MPlayerName), buffer);
  std::strncpy(MPlayerGameName, MPlayerName, sizeof(MPlayerGameName) - 1);
  MPlayerGameName[sizeof(MPlayerGameName) - 1] = '\0';

  MPlayerColorIdx = WWGetPrivateProfileInt("Options", "Color", 0, buffer);
  MPlayerPrefColor = MPlayerColorIdx;
  MPlayerHouse = static_cast<HousesType>(WWGetPrivateProfileInt("Options", "Side", HOUSE_GOOD, buffer));

  MPlayerCredits = WWGetPrivateProfileInt("Options", "Credits", 0, buffer);
  MPlayerBases = WWGetPrivateProfileInt("Options", "Bases", 0, buffer);
  MPlayerTiberium = WWGetPrivateProfileInt("Options", "Tiberium", 0, buffer);
  MPlayerGoodies = WWGetPrivateProfileInt("Options", "Crates", 0, buffer);
  MPlayerGhosts = WWGetPrivateProfileInt("Options", "AI", 0, buffer);
  BuildLevel = WWGetPrivateProfileInt("Options", "BuildLevel", 0, buffer);
  MPlayerUnitCount = WWGetPrivateProfileInt("Options", "UnitCount", 0, buffer);
  Seed = WWGetPrivateProfileInt("Options", "Seed", 0, buffer);
  Special.IsCaptureTheFlag = WWGetPrivateProfileInt("Options", "CaptureTheFlag", 0, buffer);
  PlanetWestwoodGameID = static_cast<unsigned long>(WWGetPrivateProfileInt("Internet", "GameID", 0, buffer));
  PlanetWestwoodStartTime = static_cast<unsigned long>(WWGetPrivateProfileInt("Internet", "StartTime", 0, buffer));
  InternetMaxPlayers = WWGetPrivateProfileInt("Internet", "MaxPlayers", 2, buffer);

  if (MPlayerTiberium) {
    Special.IsTGrowth = 1;
    Special.IsTSpread = 1;
  } else {
    Special.IsTGrowth = 0;
    Special.IsTSpread = 0;
  }

  ScenarioIdx = WWGetPrivateProfileInt("Options", "Scenario", 0, buffer);
  Scenario = ScenarioIdx;
  Options.GameSpeed = 0;
  MPlayerLocalID = Build_MPlayerID(MPlayerColorIdx, MPlayerHouse);

  MPlayerMaxAhead = WChatMaxAhead =
      static_cast<unsigned long>(WWGetPrivateProfileInt("Timing", "MaxAhead", 9, buffer));
  FrameSendRate = WChatSendRate = WWGetPrivateProfileInt("Timing", "SendRate", 3, buffer);

  return 1;
}

bool Is_User_WChat_Registered(char* buffer, int buffer_len) {
  if (!buffer || buffer_len <= 0) return false;
  buffer[0] = '\0';

  const char* env_user = std::getenv("TD_WCHAT_USER");
  if (env_user && *env_user) {
    std::strncpy(buffer, env_user, static_cast<std::size_t>(buffer_len - 1));
    buffer[buffer_len - 1] = '\0';
    return true;
  }

  Read_Profile_String("Internet", "Handle", "", buffer, buffer_len);
  return buffer[0] != '\0';
}

bool Spawn_WChat(bool can_launch) {
  if (!can_launch) return true;
  return Launch_Command_If_Configured("TD_WCHAT_CMD");
}

bool Spawn_Registration_App(void) { return Launch_Command_If_Configured("TD_REGISTER_CMD"); }

bool Do_The_Internet_Menu_Thang(void) {
#ifndef DEMO
  const int factor = (SeenBuff.Get_Width() == 320) ? 1 : 2;

  const int d_dialog_w = 120 * factor;
  const int d_dialog_h = 80 * factor;
  const int d_dialog_x = ((320 * factor - d_dialog_w) / 2);
  const int d_dialog_y = ((200 * factor - d_dialog_h) / 2);
  const int d_dialog_cx = d_dialog_x + (d_dialog_w / 2);

  const int d_cancel_w = 40 * factor;
  const int d_cancel_h = 9 * factor;
  const int d_cancel_x = d_dialog_cx - d_cancel_w / 2;
  const int d_cancel_y = d_dialog_y + d_dialog_h - 20 * factor;

  char packet[10] = {"Hello"};

  enum {
    BUTTON_CANCEL = 100,
  };

  TextButtonClass cancelbtn(BUTTON_CANCEL, TXT_CANCEL,
                            TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW, d_cancel_x,
                            d_cancel_y, d_cancel_w, d_cancel_h);

  bool process = true;
  bool display = true;

  // If the user configured an external WChat command, keep the original flow of trying to
  // launch/switch to it when the user clicks "Internet".
  if (!Special.IsFromWChat && !SpawnedFromWChat) {
    if (std::getenv("TD_WCHAT_CMD") != nullptr) {
      char users_name[256]{};
      if (Is_User_WChat_Registered(users_name, static_cast<int>(sizeof(users_name)))) {
        GameStatisticsPacketSent = false;
        if (!Spawn_WChat(true)) {
          Set_Logic_Page(SeenBuff);
          Load_Title_Screen(const_cast<char*>("HTITLE.PCX"), &HidPage, Palette);
          HidPage.Blit(SeenBuff);
          Set_Palette(Palette);
          CCMessageBox().Process(TXT_ERROR_UNABLE_TO_RUN_WCHAT, TXT_OK);
          LogicPage->Clear();
          return false;
        }
      } else {
        Set_Logic_Page(SeenBuff);
        Load_Title_Screen(const_cast<char*>("HTITLE.PCX"), &HidPage, Palette);
        HidPage.Blit(SeenBuff);
        Set_Palette(Palette);
        if (CCMessageBox().Process(TXT_EXPLAIN_REGISTRATION, TXT_REGISTER, TXT_CANCEL)) {
          LogicPage->Clear();
          return false;
        }
        LogicPage->Clear();
        Spawn_Registration_App();
        return false;
      }
    }
  }

  while (process) {
    if (AllSurfaces.SurfacesRestored) {
      AllSurfaces.SurfacesRestored = FALSE;
      display = true;
    }

    if (display) {
      Set_Logic_Page(SeenBuff);
      Hide_Mouse();

      Load_Title_Screen(const_cast<char*>("HTITLE.PCX"), &HidPage, Palette);
      HidPage.Blit(SeenBuff);
      Set_Palette(Palette);

      Dialog_Box(d_dialog_x, d_dialog_y, d_dialog_w, d_dialog_h);
      Draw_Caption(TXT_NONE, d_dialog_x, d_dialog_y, d_dialog_w);

      Fancy_Text_Print(TXT_CONNECTING, d_dialog_cx, d_dialog_y + 25 * factor, CC_GREEN, TBLACK,
                       TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW | TPF_CENTER);

      cancelbtn.Draw_Me(true);

      Show_Mouse();
      display = false;
    }

    if (DDEServer.Get_MPlayer_Game_Info()) {
      return true;
    }

    const KeyNumType input = cancelbtn.Input();
    switch (input) {
      case KN_ESC:
      case (BUTTON_CANCEL | KN_BUTTON):
        process = false;
        Send_Data_To_DDE_Server(packet, static_cast<int>(std::strlen(packet)),
                                DDEServerClass::DDE_CONNECTION_FAILED);
        GameStatisticsPacketSent = false;
        Spawn_WChat(false);
        break;
      default:
        break;
    }
  }
#endif

  return false;
}

