#include "ipx95.h"
#include "ipx.h"
#include "ipxaddr.h"
#include "ipxconn.h"

#include <algorithm>
#include <cstring>

#if defined(_WIN32)
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

namespace {

constexpr int kMaxReceiveBuffer = 2048;

}  // namespace

bool IPX_Initialise(void) {
	// The real IPX stack init is replaced by the UDP-backed IPXConnClass.
	// Keep success semantics so higher-level multiplayer code can proceed.
	return true;
}

bool IPX_Get_Outstanding_Buffer95(unsigned char* buffer) {
	if (!buffer) return false;

	IPXConnClass::Pump();
	IPXAddressClass from_address;
	int length = 0;
	if (!IPXConnClass::Peek_Packet(&from_address, &length)) {
		return false;
	}

	if (length < 0) {
		return false;
	}

	if (static_cast<std::size_t>(length) > (kMaxReceiveBuffer - sizeof(IPXHeaderType))) {
		// Drop oversized datagrams to avoid overflowing the legacy fixed buffer.
		// The original code path uses a fixed temp buffer and expects packets
		// below MTU size.
		IPXAddressClass discard_address;
		IPXConnClass::Pop_Packet(nullptr, 0, &discard_address);
		return false;
	}

	auto* header = reinterpret_cast<IPXHeaderType*>(buffer);
	std::memset(header, 0, sizeof(*header));
	header->CheckSum = 0xFFFF;
	header->Length = htons(static_cast<std::uint16_t>(sizeof(IPXHeaderType) + length));
	header->SourceNetworkSocket = htons(IPXConnClass::Socket);
	std::memcpy(header->SourceNetworkNumber, from_address.NetworkNumber, sizeof(from_address.NetworkNumber));
	std::memcpy(header->SourceNetworkNode, from_address.NodeAddress, sizeof(from_address.NodeAddress));

	const int copied =
	    IPXConnClass::Pop_Packet(reinterpret_cast<char*>(buffer + sizeof(IPXHeaderType)), length, &from_address);
	if (copied <= 0) {
		return false;
	}
	header->Length = htons(static_cast<std::uint16_t>(sizeof(IPXHeaderType) + copied));
	return true;
}

void IPX_Shut_Down95(void) {}

int IPX_Send_Packet95(unsigned char* send_address, unsigned char* buf, int buflen, unsigned char* net,
                      unsigned char* node) {
	if (!buf || buflen <= 0) return 0;

	IPXAddressClass destination;
	if (net) {
		std::memcpy(destination.NetworkNumber, net, sizeof(destination.NetworkNumber));
	}
	if (node) {
		std::memcpy(destination.NodeAddress, node, sizeof(destination.NodeAddress));
	}

	NetNodeType immed{};
	if (send_address) {
		std::memcpy(immed, send_address, sizeof(immed));
	}

	return IPXConnClass::Send_To(reinterpret_cast<char*>(buf), buflen, &destination, immed);
}

int IPX_Broadcast_Packet95(unsigned char* buf, int buflen) {
	if (!buf || buflen <= 0) return 0;
	return IPXConnClass::Broadcast(reinterpret_cast<char*>(buf), buflen);
}

bool IPX_Start_Listening95(void) { return IPXConnClass::Start_Listening(); }

int IPX_Open_Socket95(int socket) { return IPX_Open_Socket(static_cast<unsigned short>(socket)); }

void IPX_Close_Socket95(int socket) { IPX_Close_Socket(static_cast<unsigned short>(socket)); }

int IPX_Get_Connection_Number95(void) { return IPX_Get_Connection_Number(); }

int IPX_Get_Local_Target95(unsigned char* dest_network, unsigned char* dest_node, unsigned short dest_socket, unsigned char* bridge_address) {
	return IPX_Get_Local_Target(dest_network, dest_node, dest_socket, bridge_address);
}
