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

#include <cstring>

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

void IPXConnClass::Init(void) {}

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
}

bool IPXConnClass::Start_Listening(void) {
	Listening = 1;
	return true;
}

bool IPXConnClass::Stop_Listening(void) {
	Listening = 0;
	return true;
}

int IPXConnClass::Send(char*, int buflen) { return buflen; }

int IPXConnClass::Open_Socket(unsigned short socket) {
	Socket = socket;
	SocketOpen = 1;
	return 1;
}

void IPXConnClass::Close_Socket(unsigned short socket) {
	if (Socket == socket) {
		SocketOpen = 0;
	}
}

int IPXConnClass::Send_To(char*, int buflen, IPXAddressClass*, NetNodeType) { return buflen; }

int IPXConnClass::Broadcast(char*, int buflen) { return buflen; }
