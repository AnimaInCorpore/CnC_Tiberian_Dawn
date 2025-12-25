#include "dde.h"
#include <cstring>
#include <cstdlib>
#include <cstdint>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

namespace {
int Get_Env_Port(const char* name, int fallback) {
    const char* value = std::getenv(name);
    if (!value) return fallback;
    int port = std::atoi(value);
    if (port <= 0 || port > 65535) return fallback;
    return port;
}

int Make_Udp_Socket() {
    int sock = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) return -1;
    return sock;
}

bool Bind_Loopback(int sock, int port) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<uint16_t>(port));
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    return ::bind(sock, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)) == 0;
}

bool Send_Loopback(int sock, int port, const void* data, std::size_t len) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<uint16_t>(port));
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    const ssize_t sent = ::sendto(sock, data, len, 0, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr));
    return sent == static_cast<ssize_t>(len);
}
}  // namespace

DWORD Instance_Class::id_inst = 1;
BOOL Instance_Class::process_pokes = FALSE;
char Instance_Class::ascii_name[32] = {0};
BOOL CALLBACK (*Instance_Class::callback)(LPBYTE, long) = NULL;

Instance_Class::Instance_Class(LPSTR name1, LPSTR name2)
{
    // Cross-platform DDE replacement: use a localhost UDP socket for basic
    // client/server pokes. This preserves the original API surface without
    // depending on the Windows DDEML.
    std::strncpy(ascii_name, name1 ? name1 : "", sizeof(ascii_name) - 1);
    ascii_name[sizeof(ascii_name) - 1] = '\0';
    remote_name = nullptr;
    local_name = nullptr;
    system_topic = nullptr;
    poke_topic = nullptr;
    poke_item = nullptr;
    conv_handle = 0;
    dde_error = FALSE;

    (void)name2;
}

Instance_Class::~Instance_Class()
{
    if (conv_handle) {
        int fd = static_cast<int>(reinterpret_cast<std::intptr_t>(conv_handle));
        if (fd >= 0) {
            ::close(fd);
        }
        conv_handle = 0;
    }
}

BOOL Instance_Class::Enable_Callback(BOOL flag)
{
    process_pokes = flag;
    return process_pokes;
}

BOOL Instance_Class::Register_Server(BOOL CALLBACK (*callback_fnc)(LPBYTE, long))
{
    callback = callback_fnc;
    if (!callback) {
        return FALSE;
    }

    // Bind a loopback UDP socket for incoming pokes. Default port matches the
    // sender default so running the game alone is harmless.
    const int listen_port = Get_Env_Port("TD_DDE_LISTEN_PORT", Get_Env_Port("TD_DDE_PORT", 48765));
    int sock = Make_Udp_Socket();
    if (sock < 0) {
        return FALSE;
    }
    if (!Bind_Loopback(sock, listen_port)) {
        ::close(sock);
        return FALSE;
    }
    conv_handle = reinterpret_cast<HCONV>(static_cast<std::intptr_t>(sock));
    return TRUE;
}

BOOL Instance_Class::Test_Server_Running(HSZ)
{
    // Probe by sending an empty datagram to the configured port.
    int sock = Make_Udp_Socket();
    if (sock < 0) return FALSE;
    const int port = Get_Env_Port("TD_DDE_PORT", 48765);
    const char probe[1] = {0};
    const bool ok = Send_Loopback(sock, port, probe, sizeof(probe));
    ::close(sock);
    return ok ? TRUE : FALSE;
}

BOOL Instance_Class::Open_Poke_Connection(HSZ)
{
    if (conv_handle) {
        return TRUE;
    }
    int sock = Make_Udp_Socket();
    if (sock < 0) {
        return FALSE;
    }
    conv_handle = reinterpret_cast<HCONV>(static_cast<std::intptr_t>(sock));
    return TRUE;
}

BOOL Instance_Class::Close_Poke_Connection(void)
{
    if (conv_handle) {
        int fd = static_cast<int>(reinterpret_cast<std::intptr_t>(conv_handle));
        if (fd >= 0) {
            ::close(fd);
        }
        conv_handle = 0;
    }
    return TRUE;
}

BOOL Instance_Class::Poke_Server(LPBYTE poke_data, DWORD poke_length)
{
    if (!conv_handle) {
        if (!Open_Poke_Connection(nullptr)) return FALSE;
    }
    const int port = Get_Env_Port("TD_DDE_PORT", 48765);
    // The legacy caller uses a separate CCDDE header for packet framing; here
    // we just forward raw bytes and leave framing to higher layers.
    // Since this API doesn't include a packet type, it is up to the caller to
    // embed it in the payload.
    const void* payload = poke_data;
    std::size_t size = static_cast<std::size_t>(poke_length);
    if (!payload || size == 0) {
        payload = "";
        size = 0;
    }
    int fd = static_cast<int>(reinterpret_cast<std::intptr_t>(conv_handle));
    if (fd < 0) return FALSE;
    return Send_Loopback(fd, port, payload, size) ? TRUE : FALSE;
}

HDDEDATA CALLBACK Instance_Class::dde_callback(UINT, UINT, HCONV, HSZ, HSZ, HDDEDATA, DWORD, DWORD)
{
    // The Windows build routes DDE events through this callback. In the port,
    // delivery is handled by the higher-level CCDDE server shim; keep the hook
    // present to satisfy the legacy API.
    return (HDDEDATA)NULL;
}
