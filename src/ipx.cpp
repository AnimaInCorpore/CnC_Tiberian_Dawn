#include "ipx.h"
#include "ipxconn.h"

#include <algorithm>
#include <cstring>
#include <vector>

int IPX_SPX_Installed(void) {
	// Always report availability so higher-level logic can continue to run.
	return 1;
}

int IPX_Open_Socket(unsigned short socket) { return IPXConnClass::Open_Socket(socket); }

int IPX_Close_Socket(unsigned short socket) {
	IPXConnClass::Close_Socket(socket);
	return 0;
}

int IPX_Get_Connection_Number(void) { return 1; }

int IPX_Get_1st_Connection_Num(char* username) {
	if (username) {
		username[0] = '\0';
	}
	return 1;
}

int IPX_Get_Internet_Address(int, unsigned char* network_number, unsigned char* physical_node) {
	if (network_number) {
		std::memset(network_number, 0, 4);
	}
	if (physical_node) {
		std::memset(physical_node, 0, 6);
	}
	return 0;
}

int IPX_Get_User_ID(int, char* user_id) {
	if (user_id) {
		user_id[0] = '\0';
	}
	return 0;
}

int IPX_Listen_For_Packet(ECBType* ecb_ptr) {
	if (!ecb_ptr) return 0;
	IPXConnClass::Pump();
	IPXAddressClass address;
	int length = 0;
	if (!IPXConnClass::Peek_Packet(&address, &length)) {
		return 0;
	}

	std::vector<char> buffer(static_cast<std::size_t>(length));
	IPXConnClass::Pop_Packet(buffer.data(), length, &address);

	int copied = 0;
	for (int i = 0; i < static_cast<int>(ecb_ptr->PacketCount) && copied < length; ++i) {
		const int to_copy =
		    std::min(static_cast<int>(ecb_ptr->Packet[i].Length), length - copied);
		std::memcpy(ecb_ptr->Packet[i].Address, buffer.data() + copied, static_cast<std::size_t>(to_copy));
		copied += to_copy;
	}

	ecb_ptr->InUse = 0;
	ecb_ptr->CompletionCode = 0;
	return copied;
}

void IPX_Send_Packet(ECBType* ecb_ptr) {
	if (!ecb_ptr || ecb_ptr->PacketCount == 0) return;

	int total_len = 0;
	for (int i = 0; i < static_cast<int>(ecb_ptr->PacketCount); ++i) {
		total_len += ecb_ptr->Packet[i].Length;
	}
	std::vector<char> payload(static_cast<std::size_t>(total_len));
	int offset = 0;
	for (int i = 0; i < static_cast<int>(ecb_ptr->PacketCount); ++i) {
		std::memcpy(payload.data() + offset, ecb_ptr->Packet[i].Address, ecb_ptr->Packet[i].Length);
		offset += ecb_ptr->Packet[i].Length;
	}

	IPXAddressClass destination;
	std::memcpy(destination.NodeAddress, ecb_ptr->ImmediateAddress, sizeof(NetNodeType));
	std::memset(destination.NetworkNumber, 0, sizeof(destination.NetworkNumber));

	IPXConnClass::Send_To(payload.data(), total_len, &destination, ecb_ptr->ImmediateAddress);
	ecb_ptr->InUse = 0;
	ecb_ptr->CompletionCode = 0;
}

int IPX_Get_Local_Target(unsigned char* dest_network, unsigned char* dest_node, unsigned short, unsigned char* bridge_address) {
	if (bridge_address && dest_node) {
		std::memcpy(bridge_address, dest_node, 6);
	}
	if (dest_network) {
		std::memset(dest_network, 0, 4);
	}
	return 0;
}

int IPX_Cancel_Event(ECBType*) { return 0; }

void Let_IPX_Breath(void) { IPXConnClass::Pump(); }
