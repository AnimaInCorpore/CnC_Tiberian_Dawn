/*
**	Command & Conquer(tm)
**	Copyright 2025 Electronic Arts Inc.
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "legacy/tcpip.h"

#include <algorithm>
#include <array>
#include <cerrno>
#include <cstring>
#include <deque>
#include <string>
#include <vector>

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

namespace {

#if defined(_WIN32)
using socket_len_t = int;
constexpr SOCKET kInvalidSocket = INVALID_SOCKET;
bool g_wsa_ready = false;

bool Ensure_WSA() {
	if (g_wsa_ready) return true;
	WSADATA data{};
	if (WSAStartup(MAKEWORD(WINSOCK_MAJOR_VER, WINSOCK_MINOR_VER), &data) != 0) {
		return false;
	}
	g_wsa_ready = true;
	return true;
}

void Close_Native(SOCKET s) {
	if (s != INVALID_SOCKET) {
		closesocket(s);
	}
}

bool Set_Non_Blocking(SOCKET s) {
	u_long mode = 1;
	return ioctlsocket(s, FIONBIO, &mode) == 0;
}

int Last_Error() { return WSAGetLastError(); }
#else
using socket_len_t = socklen_t;
constexpr int kInvalidSocket = -1;

bool Ensure_WSA() { return true; }

void Close_Native(int s) {
	if (s != kInvalidSocket) {
		close(s);
	}
}

bool Set_Non_Blocking(int s) {
	int flags = fcntl(s, F_GETFL, 0);
	if (flags < 0) return false;
	return fcntl(s, F_SETFL, flags | O_NONBLOCK) == 0;
}

int Last_Error() { return errno; }
#endif

constexpr std::size_t kMaxDatagram = WS_RECEIVE_BUFFER_LEN;

struct Datagram {
	std::vector<char> payload;
	sockaddr_in from{};
};

SOCKET g_socket = kInvalidSocket;
std::deque<Datagram> g_incoming;
std::vector<sockaddr_in> g_clients;
sockaddr_in g_server_address{};
bool g_has_server_address = false;

bool Addresses_Equal(sockaddr_in const& a, sockaddr_in const& b) {
	return a.sin_port == b.sin_port && a.sin_addr.s_addr == b.sin_addr.s_addr;
}

bool Ensure_Socket_Open(unsigned short port) {
	if (g_socket != kInvalidSocket) return true;
	if (!Ensure_WSA()) return false;
	auto fd = ::socket(AF_INET, SOCK_DGRAM, 0);
	if (fd == kInvalidSocket) return false;

	int reuse = 1;
	::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&reuse), sizeof(reuse));
	int broadcast = 1;
	::setsockopt(fd, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<char*>(&broadcast), sizeof(broadcast));

	sockaddr_in local{};
	local.sin_family = AF_INET;
	local.sin_port = htons(port);
	local.sin_addr.s_addr = htonl(INADDR_ANY);

	if (::bind(fd, reinterpret_cast<sockaddr*>(&local), sizeof(local)) == SOCKET_ERROR) {
		Close_Native(fd);
		return false;
	}

	if (!Set_Non_Blocking(fd)) {
		Close_Native(fd);
		return false;
	}

	g_socket = fd;
	return true;
}

sockaddr_in Build_Address(char const* host, unsigned short port) {
	sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	if (host && inet_pton(AF_INET, host, &addr.sin_addr) == 1) {
		return addr;
	}
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	return addr;
}

void Clear_Queues() {
	g_incoming.clear();
	g_clients.clear();
	g_has_server_address = false;
}

}  // namespace

bool Server = false;
TcpipManagerClass Winsock;
char PlanetWestwoodIPAddress[IP_ADDRESS_MAX] = {};
long PlanetWestwoodPortNumber = PORTNUM;
bool PlanetWestwoodIsHost = false;
bool UseVirtualSubnetServer = false;
int InternetMaxPlayers = 4;
GameStatisticsStruct GameStatistics{};

TcpipManagerClass::TcpipManagerClass(void)
    : WinsockInitialised(FALSE),
      ListenSocket(kInvalidSocket),
      ConnectSocket(kInvalidSocket),
      UDPSocket(kInvalidSocket),
      Async(nullptr),
      ClientName{},
      ReceiveBuffer{},
      IsServer(FALSE),
      Connected(FALSE),
      HostAddress{},
      ConnectStatus(NOT_CONNECTING),
      UseUDP(FALSE),
      UDPIPAddress{},
      SocketReceiveBuffer(0),
      SocketSendBuffer(0),
      TXBufferHead(0),
      TXBufferTail(0),
      RXBufferHead(0),
      RXBufferTail(0) {}

TcpipManagerClass::~TcpipManagerClass(void) { Close(); }

BOOL TcpipManagerClass::Init(void) {
	if (!Ensure_WSA()) return FALSE;
	WinsockInitialised = TRUE;
	ConnectStatus = NOT_CONNECTING;
	const unsigned short port = static_cast<unsigned short>(UseUDP ? UDP_PORT : PlanetWestwoodPortNumber);
	if (Ensure_Socket_Open(port)) {
		ListenSocket = UDPSocket = g_socket;
	}
	return WinsockInitialised;
}

void TcpipManagerClass::Start_Server(void) {
	const unsigned short port = static_cast<unsigned short>(UseUDP ? UDP_PORT : PlanetWestwoodPortNumber);
	if (!Ensure_Socket_Open(port)) return;
	IsServer = TRUE;
	Connected = TRUE;
	ConnectStatus = CONNECTED_OK;
	ListenSocket = UDPSocket = g_socket;
	Clear_Queues();
	SocketReceiveBuffer = 0;
	SocketSendBuffer = 0;
}

void TcpipManagerClass::Start_Client(void) {
	const unsigned short port = static_cast<unsigned short>(UseUDP ? UDP_PORT : PlanetWestwoodPortNumber);
	if (!Ensure_Socket_Open(port)) return;
	IsServer = FALSE;
	Connected = TRUE;
	ConnectStatus = CONNECTED_OK;
	ConnectSocket = UDPSocket = g_socket;
	if (HostAddress[0] != '\0') {
		g_server_address = Build_Address(HostAddress, port);
		g_has_server_address = true;
	}
	Clear_Queues();
	SocketReceiveBuffer = 0;
	SocketSendBuffer = 0;
}

void TcpipManagerClass::Close_Socket(SOCKET s) {
	if (g_socket != kInvalidSocket && s == g_socket) {
		Close_Native(g_socket);
		g_socket = kInvalidSocket;
	}
}

void TcpipManagerClass::Message_Handler(HWND, UINT, UINT, LONG) {}

void TcpipManagerClass::Copy_To_In_Buffer(int bytes) { SocketReceiveBuffer += bytes; }

int TcpipManagerClass::Read(void* buffer, int buffer_len) {
	if (!buffer || buffer_len <= 0) return 0;
	if (g_socket != kInvalidSocket) {
		std::array<char, kMaxDatagram> recv_buffer{};
		sockaddr_in from{};
		socket_len_t from_len = sizeof(from);

		while (true) {
			const int received =
			    ::recvfrom(g_socket, recv_buffer.data(), static_cast<int>(recv_buffer.size()), 0,
			               reinterpret_cast<sockaddr*>(&from), &from_len);
			if (received <= 0) {
#if defined(_WIN32)
				if (Last_Error() == WSAEWOULDBLOCK) {
					break;
				}
#else
				if (errno == EAGAIN || errno == EWOULDBLOCK) {
					break;
				}
#endif
				break;
			}

			if (IsServer) {
				auto it = std::find_if(g_clients.begin(), g_clients.end(),
				                       [&](sockaddr_in const& client) { return Addresses_Equal(client, from); });
				if (it == g_clients.end()) {
					g_clients.push_back(from);
				}
			}

			g_incoming.push_back({std::vector<char>(recv_buffer.begin(), recv_buffer.begin() + received), from});
			SocketReceiveBuffer += received;
		}
	}
	if (g_incoming.empty()) return 0;
	const auto message = std::move(g_incoming.front());
	g_incoming.pop_front();
	const int to_copy = std::min(buffer_len, static_cast<int>(message.payload.size()));
	std::memcpy(buffer, message.payload.data(), static_cast<std::size_t>(to_copy));
	if (SocketReceiveBuffer >= to_copy) {
		SocketReceiveBuffer -= to_copy;
	}
	return to_copy;
}

void TcpipManagerClass::Write(void* buffer, int buffer_len) {
	if (!buffer || buffer_len <= 0) return;
	const unsigned short port = static_cast<unsigned short>(UseUDP ? UDP_PORT : PlanetWestwoodPortNumber);
	if (!Ensure_Socket_Open(port)) return;

	const char* data = static_cast<const char*>(buffer);
	sockaddr_in destination{};

	if (IsServer) {
		for (auto const& client : g_clients) {
			::sendto(g_socket, data, buffer_len, 0, reinterpret_cast<const sockaddr*>(&client), sizeof(client));
		}
	} else {
		if (!g_has_server_address) {
			g_server_address = Build_Address(PlanetWestwoodIPAddress[0] ? PlanetWestwoodIPAddress : nullptr, port);
			g_has_server_address = true;
		}
		destination = g_server_address;
		::sendto(g_socket, data, buffer_len, 0, reinterpret_cast<sockaddr*>(&destination), sizeof(destination));
	}

	SocketSendBuffer += buffer_len;
}

BOOL TcpipManagerClass::Add_Client(void) {
	Connected = TRUE;
	ConnectStatus = CONNECTED_OK;
	return TRUE;
}

void TcpipManagerClass::Close(void) {
	Connected = FALSE;
	ConnectStatus = NOT_CONNECTING;
	Close_Socket(g_socket);
	ListenSocket = ConnectSocket = UDPSocket = kInvalidSocket;
	Clear_Queues();
	SocketReceiveBuffer = 0;
	SocketSendBuffer = 0;
#if defined(_WIN32)
	if (g_wsa_ready) {
		WSACleanup();
		g_wsa_ready = false;
	}
#endif
}

void TcpipManagerClass::Set_Host_Address(char* address) {
	if (!address) return;
	std::strncpy(HostAddress, address, sizeof(HostAddress) - 1);
	HostAddress[sizeof(HostAddress) - 1] = '\0';
	const unsigned short port = static_cast<unsigned short>(UseUDP ? UDP_PORT : PlanetWestwoodPortNumber);
	g_server_address = Build_Address(HostAddress, port);
	g_has_server_address = true;
}

void TcpipManagerClass::Set_Protocol_UDP(BOOL state) { UseUDP = state; }

void TcpipManagerClass::Clear_Socket_Error(SOCKET) { (void)Last_Error(); }
