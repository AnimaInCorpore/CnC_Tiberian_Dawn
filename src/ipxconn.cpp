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

#include "ipxconn.h"

#include <algorithm>
#include <array>
#include <cerrno>
#include <cstring>
#include <deque>
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

#ifndef SOCKET_ERROR
#define SOCKET_ERROR (-1)
#endif

unsigned short IPXConnClass::Socket = 0;
int IPXConnClass::ConnectionNum = 0;
int IPXConnClass::PacketLen = 0;
ECBType* IPXConnClass::ListenECB = nullptr;
IPXHeaderType* IPXConnClass::ListenHeader = nullptr;
char* IPXConnClass::ListenBuf = nullptr;
ECBType* IPXConnClass::SendECB = nullptr;
IPXHeaderType* IPXConnClass::SendHeader = nullptr;
char* IPXConnClass::SendBuf = nullptr;
long IPXConnClass::Handler = 0;
int IPXConnClass::Configured = 0;
int IPXConnClass::SocketOpen = 0;
int IPXConnClass::Listening = 0;

namespace {

#if defined(_WIN32)
using socket_len_t = int;
constexpr SOCKET kInvalidSocket = INVALID_SOCKET;
void Close_Native(SOCKET s) {
	if (s != INVALID_SOCKET) {
		closesocket(s);
	}
}

bool Set_Non_Blocking(SOCKET s) {
	u_long mode = 1;
	return ioctlsocket(s, FIONBIO, &mode) == 0;
}

[[maybe_unused]] int Last_Socket_Error() { return WSAGetLastError(); }
#else
using socket_len_t = socklen_t;
using socket_t = int;
constexpr socket_t kInvalidSocket = -1;
void Close_Native(socket_t s) {
	if (s != kInvalidSocket) {
		close(s);
	}
}

bool Set_Non_Blocking(socket_t s) {
	int flags = fcntl(s, F_GETFL, 0);
	if (flags < 0) return false;
	return fcntl(s, F_SETFL, flags | O_NONBLOCK) == 0;
}

[[maybe_unused]] int Last_Socket_Error() { return errno; }
#endif

constexpr unsigned short kIpxPortBase = 0x5000;
constexpr std::size_t kMaxDatagram = 1500;

#if defined(_WIN32)
SOCKET g_socket = kInvalidSocket;
#else
socket_t g_socket = kInvalidSocket;
#endif

using Payload = std::pair<IPXAddressClass, std::vector<char>>;
std::deque<Payload> g_incoming;

sockaddr_in ToSockAddr(IPXAddressClass const& address, unsigned short socket_id) {
	sockaddr_in sa{};
	sa.sin_family = AF_INET;
	sa.sin_port = htons(static_cast<unsigned short>(kIpxPortBase + socket_id));
	std::uint32_t ip = 0;
	std::memcpy(&ip, address.NodeAddress, sizeof(ip));
	if (ip == 0) {
		ip = htonl(INADDR_LOOPBACK);
	}
	sa.sin_addr.s_addr = ip;
	return sa;
}

IPXAddressClass FromSockAddr(sockaddr_in const& sa) {
	IPXAddressClass address;
	std::memset(address.NetworkNumber, 0, sizeof(address.NetworkNumber));
	std::uint32_t ip = sa.sin_addr.s_addr;
	std::memcpy(address.NodeAddress, &ip, sizeof(ip));
	const unsigned short port = ntohs(sa.sin_port);
	address.NodeAddress[4] = static_cast<std::uint8_t>((port >> 8) & 0xFF);
	address.NodeAddress[5] = static_cast<std::uint8_t>(port & 0xFF);
	return address;
}

bool Ensure_Socket_Open(unsigned short socket_id) {
	if (g_socket != kInvalidSocket) return true;
	auto fd = ::socket(AF_INET, SOCK_DGRAM, 0);
	if (fd == kInvalidSocket) {
		return false;
	}

	int reuse = 1;
	::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&reuse), sizeof(reuse));
	int broadcast = 1;
	::setsockopt(fd, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<char*>(&broadcast), sizeof(broadcast));

	sockaddr_in local{};
	local.sin_family = AF_INET;
	local.sin_port = htons(static_cast<unsigned short>(kIpxPortBase + socket_id));
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

void Poll_Socket() {
	if (g_socket == kInvalidSocket) return;
	std::array<char, kMaxDatagram> buffer{};
	sockaddr_in from{};
	socket_len_t from_len = sizeof(from);

	while (true) {
		const int received =
		    ::recvfrom(g_socket, buffer.data(), static_cast<int>(buffer.size()), 0,
		               reinterpret_cast<sockaddr*>(&from), &from_len);
		if (received <= 0) {
#if defined(_WIN32)
			if (Last_Socket_Error() == WSAEWOULDBLOCK) {
				break;
			}
#else
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				break;
			}
#endif
			break;
		}

		g_incoming.emplace_back(FromSockAddr(from),
		                        std::vector<char>(buffer.begin(), buffer.begin() + received));
	}
}

}  // namespace

IPXConnClass::IPXConnClass(int, int, int, unsigned short socket, IPXAddressClass* address, int id, char* name)
    : Address{}, ImmediateAddress{}, Immed_Set(0), ID(id) {
	if (address) {
		Address = *address;
	}
	if (name) {
		std::strncpy(Name, name, sizeof(Name) - 1);
		Name[sizeof(Name) - 1] = '\0';
	} else {
		Name[0] = '\0';
	}
	Socket = socket;
}

void IPXConnClass::Init(void) { Ensure_Socket_Open(Socket); }

void IPXConnClass::Configure(unsigned short socket, int conn_num, ECBType* listen_ecb, ECBType* send_ecb,
                             IPXHeaderType* listen_header, IPXHeaderType* send_header, char* listen_buf,
                             char* send_buf, long handler_rm_ptr, int maxpacketlen) {
	Socket = socket;
	ConnectionNum = conn_num;
	ListenECB = listen_ecb;
	SendECB = send_ecb;
	ListenHeader = listen_header;
	SendHeader = send_header;
	ListenBuf = listen_buf;
	SendBuf = send_buf;
	Handler = handler_rm_ptr;
	PacketLen = maxpacketlen;
	Configured = 1;
	SocketOpen = Ensure_Socket_Open(socket) ? 1 : 0;
}

bool IPXConnClass::Start_Listening(void) {
	Listening = Ensure_Socket_Open(Socket) ? 1 : 0;
	return Listening != 0;
}

bool IPXConnClass::Stop_Listening(void) {
	Listening = 0;
	return true;
}

int IPXConnClass::Send(char* buf, int buflen) {
	if (!buf || buflen <= 0) return 0;
	if (Immed_Set) {
		return Send_To(buf, buflen, &Address, ImmediateAddress);
	}
	return Broadcast(buf, buflen);
}

int IPXConnClass::Open_Socket(unsigned short socket) {
	if (SocketOpen && Socket != socket && g_socket != kInvalidSocket) {
		Close_Native(g_socket);
		g_socket = kInvalidSocket;
	}
	Socket = socket;
	SocketOpen = Ensure_Socket_Open(socket) ? 1 : 0;
	return SocketOpen;
}

void IPXConnClass::Close_Socket(unsigned short socket) {
	if (Socket == socket && g_socket != kInvalidSocket) {
		Close_Native(g_socket);
		g_socket = kInvalidSocket;
	}
	SocketOpen = 0;
	Listening = 0;
	g_incoming.clear();
}

int IPXConnClass::Send_To(char* buf, int buflen, IPXAddressClass* address, NetNodeType immed) {
	if (!buf || buflen <= 0) return 0;
	IPXAddressClass target;
	if (address) {
		target = *address;
	} else {
		std::memset(target.NetworkNumber, 0, sizeof(target.NetworkNumber));
		std::memcpy(target.NodeAddress, immed, sizeof(NetNodeType));
	}

	if (!Ensure_Socket_Open(Socket)) return 0;

	sockaddr_in dest = target.Is_Broadcast() ? sockaddr_in{} : ToSockAddr(target, Socket);
	if (target.Is_Broadcast()) {
		dest.sin_family = AF_INET;
		dest.sin_port = htons(static_cast<unsigned short>(kIpxPortBase + Socket));
		dest.sin_addr.s_addr = INADDR_BROADCAST;
	}

	const int sent = ::sendto(g_socket, buf, buflen, 0, reinterpret_cast<sockaddr*>(&dest), sizeof(dest));
	return sent == SOCKET_ERROR ? 0 : sent;
}

int IPXConnClass::Broadcast(char* buf, int buflen) {
	if (!Ensure_Socket_Open(Socket)) return 0;
	IPXAddressClass address;
	std::memset(address.NetworkNumber, 0xFF, sizeof(address.NetworkNumber));
	std::memset(address.NodeAddress, 0xFF, sizeof(address.NodeAddress));
	return Send_To(buf, buflen, &address, address.NodeAddress);
}

void IPXConnClass::Pump() { Poll_Socket(); }

bool IPXConnClass::Peek_Packet(IPXAddressClass* address, int* length) {
	Poll_Socket();
	if (g_incoming.empty()) return false;
	if (address) {
		*address = g_incoming.front().first;
	}
	if (length) {
		*length = static_cast<int>(g_incoming.front().second.size());
	}
	return true;
}

int IPXConnClass::Pop_Packet(char* buf, int buflen, IPXAddressClass* address) {
	Poll_Socket();
	if (g_incoming.empty()) return 0;
	auto packet = g_incoming.front();
	g_incoming.pop_front();
	if (address) {
		*address = packet.first;
	}
	if (!buf || buflen <= 0) return 0;
	const int to_copy = std::min(buflen, static_cast<int>(packet.second.size()));
	std::memcpy(buf, packet.second.data(), static_cast<std::size_t>(to_copy));
	return to_copy;
}
