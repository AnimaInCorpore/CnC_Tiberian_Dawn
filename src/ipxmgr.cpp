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
#include "legacy/defines.h"
#include "legacy/event.h"

IPXManagerClass Ipx(
	sizeof(GlobalPacketType),                        // size of Global Channel packets
	((546 - sizeof(IPXCommHeaderType)) / sizeof(EventClass)) * sizeof(EventClass),
	10,                                              // # entries in Global Queue
	8,                                               // # entries in Private Queues
	0x8813,                                         // Socket ID #
	IPXGlobalConnClass::COMMAND_AND_CONQUER);         // Product ID #

#include "legacy/ipx95.h"

#include <algorithm>
#include <deque>
#include <cstring>
#include <vector>

namespace {

struct IncomingMessage {
std::vector<char> payload;
	IPXAddressClass address;
	int connection_id = IPXConnClass::CONNECTION_NONE;
};

std::deque<IncomingMessage> g_global_messages;
std::deque<IncomingMessage> g_private_messages;

}  // namespace

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
	if (pvt_maxlen > 0) {
		IPXConnClass::PacketLen = pvt_maxlen;
	}
	std::fill(std::begin(Connection), std::end(Connection), nullptr);
	IPXConnClass::Open_Socket(Socket);
}

IPXManagerClass::~IPXManagerClass() {
	delete GlobalChannel;
	GlobalChannel = nullptr;
	for (auto& connection : Connection) {
		delete connection;
		connection = nullptr;
	}
}

void IPXManagerClass::Init(void) {
	if (GlobalChannel == nullptr && IPXStatus) {
		GlobalChannel = new IPXGlobalConnClass(0, 0, 0, ProductID);
	}
	IPXConnClass::Open_Socket(Socket);
}

int IPXManagerClass::Is_IPX(void) const { return IPXStatus ? 1 : 0; }

void IPXManagerClass::Set_Timing(unsigned long retrydelta, unsigned long maxretries, unsigned long timeout) {
	RetryDelta = static_cast<int>(retrydelta);
	MaxRetries = static_cast<int>(maxretries);
	Timeout = static_cast<int>(timeout);
}

int IPXManagerClass::Match_Connection(IPXAddressClass const& address) const {
	for (int i = 0; i < CONNECT_MAX; ++i) {
		if (Connection[i] != nullptr) {
			IPXAddressClass mutable_addr = address;
			if (mutable_addr == Connection[i]->Address) {
				return Connection[i]->ID;
			}
		}
	}
	return IPXConnClass::CONNECTION_NONE;
}

void IPXManagerClass::Drain_Incoming() {
	IPXConnClass::Pump();
	IPXAddressClass address;
	int length = 0;
	while (IPXConnClass::Peek_Packet(&address, &length)) {
		std::vector<char> buffer(static_cast<std::size_t>(length));
		IPXConnClass::Pop_Packet(buffer.data(), length, &address);
		IncomingMessage message{std::move(buffer), address, Match_Connection(address)};
		if (message.connection_id == IPXConnClass::CONNECTION_NONE) {
			g_global_messages.push_back(std::move(message));
		} else {
			g_private_messages.push_back(std::move(message));
		}
	}
}

bool IPXManagerClass::Set_Bridge(IPXAddressClass* address) {
	(void)address;
	return true;
}

bool IPXManagerClass::Create_Connection(int id, char* name, IPXAddressClass* address) {
	if (id < 0 || id >= CONNECT_MAX || Connection[id] != nullptr) return false;
	Connection[id] = new IPXConnClass(0, 0, IPXConnClass::PacketLen, Socket, address, id, name);
	Connection[id]->Init();
	++NumConnections;
	return true;
}

bool IPXManagerClass::Delete_Connection(int id) {
	if (id < 0 || id >= CONNECT_MAX) return false;
	delete Connection[id];
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
	if (!GlobalChannel || !buflen) return 0;
	Drain_Incoming();
	if (g_global_messages.empty()) return 0;

	const IncomingMessage message = std::move(g_global_messages.front());
	g_global_messages.pop_front();
	if (address) {
		*address = message.address;
	}
	const int to_copy = std::min(*buflen, static_cast<int>(message.payload.size()));
	std::memcpy(buf, message.payload.data(), static_cast<std::size_t>(to_copy));
	*buflen = to_copy;
	if (product_id) *product_id = ProductID;
	return to_copy;
}

int IPXManagerClass::Send_Private_Message(void* buf, int buflen, int ack_req, int conn_id) {
	(void)ack_req;
	const int index = Connection_Index(conn_id);
	if (index == -1) {
		BadConnection = conn_id;
		return 0;
	}
	BadConnection = IPXConnClass::CONNECTION_NONE;
	return Connection[index]->Send(static_cast<char*>(buf), buflen);
}

int IPXManagerClass::Get_Private_Message(void* buf, int* buflen, int* conn_id) {
	if (!buflen) return 0;
	Drain_Incoming();
	if (g_private_messages.empty()) return 0;

	const IncomingMessage message = std::move(g_private_messages.front());
	g_private_messages.pop_front();
	const int to_copy = std::min(*buflen, static_cast<int>(message.payload.size()));
	if (conn_id) *conn_id = message.connection_id;
	std::memcpy(buf, message.payload.data(), static_cast<std::size_t>(to_copy));
	*buflen = to_copy;
	return to_copy;
}

int IPXManagerClass::Service(void) {
	Drain_Incoming();
	return static_cast<int>(g_global_messages.size() + g_private_messages.size());
}

int IPXManagerClass::Get_Bad_Connection(void) { return BadConnection; }

int IPXManagerClass::Global_Num_Send(void) { return static_cast<int>(g_global_messages.size()); }

int IPXManagerClass::Global_Num_Receive(void) { return static_cast<int>(g_global_messages.size()); }

int IPXManagerClass::Private_Num_Send(int) { return static_cast<int>(g_private_messages.size()); }

int IPXManagerClass::Private_Num_Receive(int) { return static_cast<int>(g_private_messages.size()); }

void IPXManagerClass::Set_Socket(unsigned short socket) {
	Socket = socket;
	IPXConnClass::Open_Socket(Socket);
}

unsigned long IPXManagerClass::Response_Time(void) { return 0; }

unsigned long IPXManagerClass::Global_Response_Time(void) { return 0; }

void IPXManagerClass::Reset_Response_Time(void) {}

void* IPXManagerClass::Oldest_Send(void) { return nullptr; }

void IPXManagerClass::Configure_Debug(int, int, int, char**, int) {}

void IPXManagerClass::Mono_Debug_Print(int, int) {}
