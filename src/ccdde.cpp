// Minimal, portable reimplementation of the original CCDDE glue.
#include "ccdde.h"
#include "legacy/externs.h"
#include "legacy/wwlib32.h"

#include <arpa/inet.h>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>



DDEServerClass::DDEServerClass()
{
  MPlayerGameInfo = nullptr;
  MPlayerGameInfoLength = 0;
  IsEnabled = true;
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

bool Send_Data_To_DDE_Server(const char* data, int length, int packet_type)
{
  if (!data || length < 0) {
    length = 0;
  }

  // Portable replacement for Windows DDE pokes: send UDP packets to localhost.
  // An external helper (e.g. a launcher/lobby) can bind this port and emulate
  // the original WChat integration.
  const char* env_port = std::getenv("TD_DDE_PORT");
  int port = env_port ? std::atoi(env_port) : 48765;
  if (port <= 0 || port > 65535) {
    port = 48765;
  }

  int sock = ::socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    return false;
  }

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(static_cast<uint16_t>(port));
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

  const uint32_t payload_len = static_cast<uint32_t>(length);
  const uint32_t packet_len = payload_len + 2u * sizeof(uint32_t);
  std::unique_ptr<unsigned char[]> buffer(new (std::nothrow) unsigned char[packet_len]);
  if (!buffer) {
    ::close(sock);
    return false;
  }

  uint32_t* header = reinterpret_cast<uint32_t*>(buffer.get());
  header[0] = htonl(packet_len);
  header[1] = htonl(static_cast<uint32_t>(packet_type));
  if (payload_len && data) {
    std::memcpy(buffer.get() + 2u * sizeof(uint32_t), data, payload_len);
  }

  const ssize_t sent = ::sendto(sock, buffer.get(), packet_len, 0,
                                reinterpret_cast<const sockaddr*>(&addr), sizeof(addr));
  ::close(sock);
  return sent == static_cast<ssize_t>(packet_len);
}
