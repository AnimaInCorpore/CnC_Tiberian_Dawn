/* Portable DDE server shim for the ported build */
#pragma once

#include <cstdint>

// This header replaces the legacy Windows DDE glue with a portable,
// cross-platform implementation that preserves the original public API
// surface used by the game.

class DDEServerClass {
public:
  DDEServerClass();
  ~DDEServerClass();

  char *Get_MPlayer_Game_Info();
  int  Get_MPlayer_Game_Info_Length() const { return MPlayerGameInfoLength; }
  bool Callback(unsigned char *data, long length);
  void Delete_MPlayer_Game_Info();
  void Enable();
  void Disable();
  int  Time_Since_Heartbeat() const;

  enum {
    DDE_PACKET_START_MPLAYER_GAME,
    DDE_PACKET_GAME_RESULTS,
    DDE_PACKET_HEART_BEAT,
    DDE_TICKLE,
    DDE_CONNECTION_FAILED
  };

private:
  char *MPlayerGameInfo = nullptr;
  int   MPlayerGameInfoLength = 0;
  bool  IsEnabled = false;
  int   LastHeartbeat = 0;
};

extern DDEServerClass DDEServer;
extern bool Send_Data_To_DDE_Server(const char *data, int length, int packet_type);
