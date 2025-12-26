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
#include <array>
#include <chrono>
#include <deque>
#include <cstring>
#include <string>
#include <vector>

namespace {

struct IncomingMessage {
std::vector<char> payload;
	IPXAddressClass address;
	int connection_id = IPXConnClass::CONNECTION_NONE;
};

std::deque<IncomingMessage> g_global_messages;
std::deque<IncomingMessage> g_private_messages;

using steady_clock_t = std::chrono::steady_clock;

struct PendingSend {
	std::vector<char> payload;
	steady_clock_t::time_point sent_at{};
	int connection_id = IPXConnClass::CONNECTION_NONE;
	bool is_global = false;
	bool awaiting_response = false;
};

struct ResponseStats {
	unsigned long mean_ms = 0;
	unsigned long max_ms = 0;
	unsigned long samples = 0;

	void Reset() {
		mean_ms = 0;
		max_ms = 0;
		samples = 0;
	}

	void Add_Sample(unsigned long ms) {
		++samples;
		if (samples == 1) {
			mean_ms = ms;
			max_ms = ms;
			return;
		}
		mean_ms = static_cast<unsigned long>(((mean_ms * (samples - 1)) + ms) / samples);
		max_ms = std::max(max_ms, ms);
	}
};

std::deque<PendingSend> g_pending_sends;

ResponseStats g_private_response;
ResponseStats g_global_response;

std::array<steady_clock_t::time_point, IPXManagerClass::CONNECT_MAX> g_last_private_send{};
steady_clock_t::time_point g_last_global_send{};
int g_global_sent = 0;
int g_global_received = 0;
int g_private_sent = 0;
int g_private_received = 0;

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
	Reset_Response_Time();
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
	// The DOS/Win95 build used this to cache a router/bridge immediate address.
	// The UDP-backed port doesn't need a separate bridge, but we preserve the
	// call surface by treating the provided address as a "known host" hint for
	// any connections that do not have an explicit address yet.
	if (!address) return false;
	for (auto& connection : Connection) {
		if (!connection) continue;
		if (connection->Address.Is_Broadcast()) {
			connection->Address = *address;
		}
	}
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
	if (ack_req) {
		g_last_global_send = steady_clock_t::now();
		g_pending_sends.push_back(
		    PendingSend{std::vector<char>(static_cast<const char*>(buf), static_cast<const char*>(buf) + buflen),
		                g_last_global_send, IPXConnClass::CONNECTION_NONE, true, true});
	}
	++g_global_sent;
	return GlobalChannel->Send_Packet(buf, buflen, address, ack_req);
}

int IPXManagerClass::Get_Global_Message(void* buf, int* buflen, IPXAddressClass* address, unsigned short* product_id) {
	if (!GlobalChannel || !buflen) return 0;
	Drain_Incoming();
	if (g_global_messages.empty()) return 0;

	const IncomingMessage message = std::move(g_global_messages.front());
	g_global_messages.pop_front();

	++g_global_received;
	if (!g_pending_sends.empty() && g_pending_sends.front().is_global && g_pending_sends.front().awaiting_response) {
		const auto now = steady_clock_t::now();
		const unsigned long delay_ms =
		    static_cast<unsigned long>(std::chrono::duration_cast<std::chrono::milliseconds>(now - g_pending_sends.front().sent_at).count());
		g_global_response.Add_Sample(delay_ms);
		g_pending_sends.front().awaiting_response = false;
		g_pending_sends.pop_front();
	}

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
	const int index = Connection_Index(conn_id);
	if (index == -1) {
		BadConnection = conn_id;
		return 0;
	}
	BadConnection = IPXConnClass::CONNECTION_NONE;
	++g_private_sent;
	if (ack_req) {
		g_last_private_send[static_cast<std::size_t>(index)] = steady_clock_t::now();
		g_pending_sends.push_back(PendingSend{
		    std::vector<char>(static_cast<const char*>(buf), static_cast<const char*>(buf) + buflen),
		    g_last_private_send[static_cast<std::size_t>(index)],
		    conn_id,
		    false,
		    true,
		});
	}
	return Connection[index]->Send(static_cast<char*>(buf), buflen);
}

int IPXManagerClass::Get_Private_Message(void* buf, int* buflen, int* conn_id) {
	if (!buflen) return 0;
	Drain_Incoming();
	if (g_private_messages.empty()) return 0;

	const IncomingMessage message = std::move(g_private_messages.front());
	g_private_messages.pop_front();
	++g_private_received;
	if (message.connection_id != IPXConnClass::CONNECTION_NONE && !g_pending_sends.empty()) {
		const auto now = steady_clock_t::now();
		for (auto it = g_pending_sends.begin(); it != g_pending_sends.end(); ++it) {
			if (!it->is_global && it->awaiting_response && it->connection_id == message.connection_id) {
				const unsigned long delay_ms =
				    static_cast<unsigned long>(std::chrono::duration_cast<std::chrono::milliseconds>(now - it->sent_at).count());
				g_private_response.Add_Sample(delay_ms);
				it->awaiting_response = false;
				g_pending_sends.erase(it);
				break;
			}
		}
	}
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

int IPXManagerClass::Global_Num_Send(void) { return g_global_sent; }

int IPXManagerClass::Global_Num_Receive(void) { return g_global_received; }

int IPXManagerClass::Private_Num_Send(int) { return g_private_sent; }

int IPXManagerClass::Private_Num_Receive(int) { return g_private_received; }

void IPXManagerClass::Set_Socket(unsigned short socket) {
	Socket = socket;
	IPXConnClass::Open_Socket(Socket);
}

unsigned long IPXManagerClass::Response_Time(void) { return g_private_response.mean_ms; }

unsigned long IPXManagerClass::Global_Response_Time(void) { return g_global_response.mean_ms; }

void IPXManagerClass::Reset_Response_Time(void) {
	g_private_response.Reset();
	g_global_response.Reset();
	g_pending_sends.clear();
	g_global_sent = 0;
	g_global_received = 0;
	g_private_sent = 0;
	g_private_received = 0;
}

void* IPXManagerClass::Oldest_Send(void) {
	for (auto& pending : g_pending_sends) {
		if (pending.awaiting_response && !pending.payload.empty()) {
			return pending.payload.data();
		}
	}
	return nullptr;
}

void IPXManagerClass::Configure_Debug(int, int, int, char**, int) {
	// The original implementation configured a debug view over queue packet
	// structures. The UDP-backed port doesn't maintain that queue structure,
	// so we intentionally keep this call as a harmless no-op.
}

void IPXManagerClass::Mono_Debug_Print(int, int) {
	// Minimal debug output for parity with legacy call sites.
	// We avoid noisy stdout logging; debug builds can set a breakpoint here.
}
