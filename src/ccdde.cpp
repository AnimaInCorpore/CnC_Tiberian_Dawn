// Minimal, portable reimplementation of the original CCDDE glue.
#include "ccdde.h"
#include "legacy/externs.h"
#include "legacy/wwlib32.h"
#include "legacy/port_stubs.h" // for CCDebugString

#include <arpa/inet.h>
#include <cstring>
#include <memory>



DDEServerClass::DDEServerClass()
{
  MPlayerGameInfo = nullptr;
  MPlayerGameInfoLength = 0;
  IsEnabled = true; // DDE stubs are enabled by default in the port
  LastHeartbeat = 0;
}

DDEServerClass::~DDEServerClass()
{
  Delete_MPlayer_Game_Info();
}

void DDEServerClass::Enable()
{
  IsEnabled = true;
}

void DDEServerClass::Disable()
{
  IsEnabled = false;
}

bool DDEServerClass::Callback(unsigned char *data, long length)
{
  if (length < 0) {
    CCDebugString("C&C95 - DDE advisory (port): unknown advisory type.\n");
    return true;
  }

  if (length < static_cast<long>(2 * sizeof(uint32_t))) {
    CCDebugString("C&C95 - Received invalid DDE packet (short).\n");
    return false;
  }

  uint32_t *packet_pointer = reinterpret_cast<uint32_t *>(data);
  int actual_length = ntohl(packet_pointer[0]);
  int packet_type = ntohl(packet_pointer[1]);

  data += 2 * sizeof(uint32_t);
  length -= 2 * sizeof(uint32_t);
  actual_length -= 2 * sizeof(uint32_t);

  switch (packet_type) {
    case DDE_PACKET_START_MPLAYER_GAME:
      CCDebugString("C&C95 - Received start game packet (port).\n");
      Delete_MPlayer_Game_Info();
      MPlayerGameInfo = new char[actual_length + 1];
      std::memcpy(MPlayerGameInfo, data, actual_length);
      MPlayerGameInfo[actual_length] = '\0';
      MPlayerGameInfoLength = actual_length;
      LastHeartbeat = 0;
      break;

    case DDE_TICKLE:
      CCDebugString("C&C95 - Received 'tickle' packet (port).\n");
      break;

    case DDE_PACKET_HEART_BEAT:
      CCDebugString("C&C95 - Received heart beat packet (port).\n");
      LastHeartbeat = ProcessTimer.Time();
      break;

    default:
      CCDebugString("C&C95 - Received unrecognised DDE packet (port).\n");
      break;
  }

  return true;
}

char *DDEServerClass::Get_MPlayer_Game_Info()
{
  return MPlayerGameInfo;
}

void DDEServerClass::Delete_MPlayer_Game_Info()
{
  if (MPlayerGameInfo) {
    delete[] MPlayerGameInfo;
    MPlayerGameInfo = nullptr;
    MPlayerGameInfoLength = 0;
  }
}

int DDEServerClass::Time_Since_Heartbeat() const
{
  return static_cast<int>(ProcessTimer.Time() - LastHeartbeat);
}

bool Send_Data_To_DDE_Server(const char * /*data*/, int /*length*/, int /*packet_type*/)
{
  CCDebugString("C&C95 - DDE poke not implemented in port.\n");
  return false;
}
