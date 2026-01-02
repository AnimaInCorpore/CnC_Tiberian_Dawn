#include "legacy/function.h"
#include "legacy/ipxmgr.h"
#include "legacy/msglist.h"

#include <cstdio>
#include <cstring>

void Destroy_Connection(int id, int error) {
  int i;
  int j;
  HousesType house;
  HouseClass* housep;
  char txt[80];

  txt[0] = '\0';
  if (error == 1) {
    std::snprintf(txt, sizeof(txt), Text_String(TXT_CONNECTION_LOST), Ipx.Connection_Name(id));
  } else if (error == 0) {
    std::snprintf(txt, sizeof(txt), Text_String(TXT_LEFT_GAME), Ipx.Connection_Name(id));
  }

  if (std::strlen(txt)) {
    Messages.Add_Message(txt, MPlayerTColors[MPlayerID_To_ColorIndex(static_cast<unsigned char>(id))],
                         TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_FULLSHADOW, 600, 0, 0);
    Map.Flag_To_Redraw(false);
  }

  Ipx.Delete_Connection(id);

  for (i = 0; i < MPlayerCount; i++) {
    if (MPlayerID[i] == static_cast<unsigned char>(id)) {
      house = MPlayerHouses[i];
      housep = HouseClass::As_Pointer(house);
      housep->IsHuman = false;
      housep->IsStarted = true;

      for (j = i; j < MPlayerCount - 1; j++) {
        MPlayerID[j] = MPlayerID[j + 1];
        MPlayerHouses[j] = MPlayerHouses[j + 1];
        std::strcpy(MPlayerNames[j], MPlayerNames[j + 1]);
        TheirProcessTime[j] = TheirProcessTime[j + 1];
      }
    }
  }

  MPlayerCount--;

  if (MPlayerCount == 1) {
    std::snprintf(txt, sizeof(txt), "%s", Text_String(TXT_JUST_YOU_AND_ME));
    Messages.Add_Message(txt, MPlayerTColors[MPlayerID_To_ColorIndex(static_cast<unsigned char>(id))],
                         TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_FULLSHADOW, 600, 0, 0);
    Map.Flag_To_Redraw(false);
  }
}

void Net_Reconnect_Dialog(int reconn, int fresh, int oldest_index, unsigned long timeval) {
  static int x;
  static int y;
  static int w;
  static int h;
  int id;
  char buf1[40] = {0};
  char buf2[40] = {0};
  char const* buf3 = "";

  int factor = (SeenBuff.Get_Width() == 320) ? 1 : 2;
  int d_txt6_h = 6 * factor + 1;
  int d_margin = 5 * factor;

  if (fresh) {
    Fancy_Text_Print("", 0, 0, CC_GREEN, TBLACK,
                     TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW);
    if (reconn) {
      id = Ipx.Connection_ID(oldest_index);
      std::snprintf(buf1, sizeof(buf1), Text_String(TXT_RECONNECTING_TO), Ipx.Connection_Name(id));
    } else {
      std::snprintf(buf1, sizeof(buf1), "%s", Text_String(TXT_WAITING_FOR_CONNECTIONS));
    }
    std::snprintf(buf2, sizeof(buf2), Text_String(TXT_TIME_ALLOWED), timeval + 1);
    buf3 = Text_String(TXT_PRESS_ESC);

    w = MAX(String_Pixel_Width(buf1), String_Pixel_Width(buf2));
    w = MAX(String_Pixel_Width(buf3), w);
    w += (d_margin * 4);
    h = (d_txt6_h * 3) + (d_margin * 6);
    x = 160 * factor - (w / 2);
    y = 100 * factor - (h / 2);

    Hide_Mouse();
    Set_Logic_Page(SeenBuff);
    Dialog_Box(x, y, w, h);

    Fancy_Text_Print(buf1, 160 * factor, y + (d_margin * 2), CC_GREEN, BLACK,
                     TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW);

    Fancy_Text_Print(buf2, 160 * factor, y + (d_margin * 2) + d_txt6_h + d_margin, CC_GREEN, BLACK,
                     TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW);

    Fancy_Text_Print(buf3, 160 * factor, y + (d_margin * 2) + (d_txt6_h + d_margin) * 2,
                     CC_GREEN, BLACK, TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW);

    Show_Mouse();
  } else {
    Hide_Mouse();
    Set_Logic_Page(SeenBuff);

    std::snprintf(buf2, sizeof(buf2), Text_String(TXT_TIME_ALLOWED), timeval + 1);
    const int pixwidth = String_Pixel_Width(buf2);
    LogicPage->Fill_Rect(160 * factor - (pixwidth / 2) - 12, y + (d_margin * 2) + d_txt6_h + d_margin,
                         160 * factor + (pixwidth / 2) + 12, y + (d_margin * 2) + d_txt6_h * 2 + d_margin,
                         TBLACK);
    Fancy_Text_Print(buf2, 160 * factor, y + (d_margin * 2) + d_txt6_h + d_margin, CC_GREEN, BLACK,
                     TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW);

    Show_Mouse();
  }
}

void Shutdown_Modem(void) {
  if (!PlaybackGame) {
    if (GameToPlay == GAME_MODEM) {
      NullModem.Hangup_Modem();
    }
  }
  NullModem.Shutdown();
}

int Reconnect_Modem(void) {
  return 0;
}

void Destroy_Null_Connection(int id, int error) {
  int i;
  int j;
  int idx;
  HousesType house;
  HouseClass* housep;
  char txt[80];

  if (MPlayerCount == 1) {
    return;
  }

  idx = -1;
  for (i = 0; i < MPlayerCount; i++) {
    if (MPlayerID[i] == static_cast<unsigned char>(id)) {
      idx = i;
      break;
    }
  }

  if (idx == -1) {
    return;
  }

  txt[0] = '\0';
  if (error == 1) {
    std::snprintf(txt, sizeof(txt), Text_String(TXT_CONNECTION_LOST), MPlayerNames[idx]);
  } else if (error == 0) {
    std::snprintf(txt, sizeof(txt), Text_String(TXT_LEFT_GAME), MPlayerNames[idx]);
  } else if (error == -1) {
    NullModem.Delete_Connection();
  }

  if (std::strlen(txt)) {
    Messages.Add_Message(txt, MPlayerTColors[MPlayerID_To_ColorIndex(static_cast<unsigned char>(id))],
                         TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_FULLSHADOW, 600, 0, 0);
    Map.Flag_To_Redraw(false);
  }

  for (i = 0; i < MPlayerCount; i++) {
    if (MPlayerID[i] == static_cast<unsigned char>(id)) {
      house = MPlayerHouses[i];
      housep = HouseClass::As_Pointer(house);
      housep->IsHuman = false;

      for (j = i; j < MPlayerCount - 1; j++) {
        MPlayerID[j] = MPlayerID[j + 1];
        MPlayerHouses[j] = MPlayerHouses[j + 1];
        std::strcpy(MPlayerNames[j], MPlayerNames[j + 1]);
        TheirProcessTime[j] = TheirProcessTime[j + 1];
      }
    }
  }

  MPlayerCount--;
}
