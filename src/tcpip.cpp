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
#include <cstring>
#include <vector>

namespace {

std::vector<std::vector<char>> g_receive_queue;

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
      ListenSocket(0),
      ConnectSocket(0),
      UDPSocket(0),
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
	WinsockInitialised = TRUE;
	ConnectStatus = NOT_CONNECTING;
	return TRUE;
}

void TcpipManagerClass::Start_Server(void) {
	IsServer = TRUE;
	Connected = TRUE;
	ConnectStatus = CONNECTED_OK;
}

void TcpipManagerClass::Start_Client(void) {
	IsServer = FALSE;
	Connected = TRUE;
	ConnectStatus = CONNECTED_OK;
}

void TcpipManagerClass::Close_Socket(SOCKET) {}

void TcpipManagerClass::Message_Handler(HWND, UINT, UINT, LONG) {}

void TcpipManagerClass::Copy_To_In_Buffer(int bytes) { SocketReceiveBuffer += bytes; }

int TcpipManagerClass::Read(void* buffer, int buffer_len) {
	if (!buffer || buffer_len <= 0 || g_receive_queue.empty()) return 0;
	const auto& payload = g_receive_queue.front();
	const int to_copy = std::min(buffer_len, static_cast<int>(payload.size()));
	std::memcpy(buffer, payload.data(), static_cast<size_t>(to_copy));
	g_receive_queue.erase(g_receive_queue.begin());
	return to_copy;
}

void TcpipManagerClass::Write(void* buffer, int buffer_len) {
	if (!buffer || buffer_len <= 0) return;
	const char* data = static_cast<const char*>(buffer);
	g_receive_queue.emplace_back(data, data + buffer_len);
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
	g_receive_queue.clear();
}

void TcpipManagerClass::Set_Host_Address(char* address) {
	if (!address) return;
	std::strncpy(HostAddress, address, sizeof(HostAddress) - 1);
	HostAddress[sizeof(HostAddress) - 1] = '\0';
}

void TcpipManagerClass::Set_Protocol_UDP(BOOL state) { UseUDP = state; }

void TcpipManagerClass::Clear_Socket_Error(SOCKET) {}
