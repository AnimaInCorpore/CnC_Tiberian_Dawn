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

#include "legacy/ipxmgr.h"

#include "legacy/ipx95.h"

#include <algorithm>
#include <cstring>

IPXManagerClass::IPXManagerClass(int glb_maxlen, int pvt_maxlen, int glb_num_packets, int pvt_num_packets,
                                 unsigned short socket, unsigned short product_id)
    : GlobalChannel(nullptr),
      IPXStatus(IPX_SPX_Installed() ? 1 : 0),
      Socket(socket),
      NumConnections(0),
      BadConnection(IPXConnClass::CONNECTION_NONE),
      RetryDelta(2),
      MaxRetries(-1),
      Timeout(60),
      ProductID(product_id) {
	(void)glb_maxlen;
	(void)pvt_maxlen;
	(void)glb_num_packets;
	(void)pvt_num_packets;
	std::fill(std::begin(Connection), std::end(Connection), nullptr);
}

IPXManagerClass::~IPXManagerClass() {
	GlobalChannel = nullptr;
	std::fill(std::begin(Connection), std::end(Connection), nullptr);
}

void IPXManagerClass::Init(void) {
	if (GlobalChannel == nullptr && IPXStatus) {
		GlobalChannel = new IPXGlobalConnClass(0, 0, 0, ProductID);
	}
}

int IPXManagerClass::Is_IPX(void) const { return IPXStatus ? 1 : 0; }

void IPXManagerClass::Set_Timing(unsigned long retrydelta, unsigned long maxretries, unsigned long timeout) {
	RetryDelta = static_cast<int>(retrydelta);
	MaxRetries = static_cast<int>(maxretries);
	Timeout = static_cast<int>(timeout);
}

bool IPXManagerClass::Set_Bridge(IPXAddressClass* address) {
	(void)address;
	return true;
}

bool IPXManagerClass::Create_Connection(int, char*, IPXAddressClass*) { return false; }

bool IPXManagerClass::Delete_Connection(int id) {
	if (id < 0 || id >= CONNECT_MAX) return false;
	Connection[id] = nullptr;
	if (NumConnections > 0) {
		--NumConnections;
	}
	return true;
}

int IPXManagerClass::Num_Connections(void) { return NumConnections; }

int IPXManagerClass::Connection_ID(int index) {
	if (index < 0 || index >= CONNECT_MAX || Connection[index] == nullptr) return IPXConnClass::CONNECTION_NONE;
	return Connection[index]->ID;
}

char* IPXManagerClass::Connection_Name(int id) {
	if (id < 0 || id >= CONNECT_MAX || Connection[id] == nullptr) return nullptr;
	return Connection[id]->Name;
}

IPXAddressClass* IPXManagerClass::Connection_Address(int id) {
	if (id < 0 || id >= CONNECT_MAX || Connection[id] == nullptr) return nullptr;
	return &Connection[id]->Address;
}

int IPXManagerClass::Connection_Index(int id) {
	for (int i = 0; i < CONNECT_MAX; ++i) {
		if (Connection[i] != nullptr && Connection[i]->ID == id) {
			return i;
		}
	}
	return -1;
}

int IPXManagerClass::Send_Global_Message(void* buf, int buflen, IPXAddressClass* address, int ack_req) {
	if (!GlobalChannel) return 0;
	return GlobalChannel->Send_Packet(buf, buflen, address, ack_req);
}

int IPXManagerClass::Get_Global_Message(void* buf, int* buflen, IPXAddressClass* address, unsigned short* product_id) {
	if (!GlobalChannel) return 0;
	return GlobalChannel->Get_Packet(buf, buflen, address, product_id);
}

int IPXManagerClass::Send_Private_Message(void*, int buflen, int, int) { return buflen; }

int IPXManagerClass::Get_Private_Message(void*, int* buflen, int* conn_id) {
	if (buflen) *buflen = 0;
	if (conn_id) *conn_id = IPXConnClass::CONNECTION_NONE;
	return 0;
}

int IPXManagerClass::Service(void) { return 0; }

int IPXManagerClass::Get_Bad_Connection(void) { return BadConnection; }

int IPXManagerClass::Global_Num_Send(void) { return 0; }

int IPXManagerClass::Global_Num_Receive(void) { return 0; }

int IPXManagerClass::Private_Num_Send(int) { return 0; }

int IPXManagerClass::Private_Num_Receive(int) { return 0; }

void IPXManagerClass::Set_Socket(unsigned short socket) { Socket = socket; }

unsigned long IPXManagerClass::Response_Time(void) { return 0; }

unsigned long IPXManagerClass::Global_Response_Time(void) { return 0; }

void IPXManagerClass::Reset_Response_Time(void) {}

void* IPXManagerClass::Oldest_Send(void) { return nullptr; }

void IPXManagerClass::Configure_Debug(int, int, int, char**, int) {}

void IPXManagerClass::Mono_Debug_Print(int, int) {}
