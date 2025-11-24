#pragma once

#include <cstdint>
#include <cstring>

using NetNumType = std::uint8_t[4];
using NetNodeType = std::uint8_t[6];
using UserID = char[48];

struct IPXHeaderType {
	std::uint16_t CheckSum = 0xFFFF;
	std::uint16_t Length = 0;
	std::uint8_t TransportControl = 0;
	std::uint8_t PacketType = 0;
	std::uint8_t DestNetworkNumber[4] = {};
	std::uint8_t DestNetworkNode[6] = {};
	std::uint16_t DestNetworkSocket = 0;
	std::uint8_t SourceNetworkNumber[4] = {};
	std::uint8_t SourceNetworkNode[6] = {};
	std::uint16_t SourceNetworkSocket = 0;
};

struct ECBType {
	void* Link_Address = nullptr;
	void (*Event_Service_Routine)(void) = nullptr;
	std::uint8_t InUse = 0;
	std::uint8_t CompletionCode = 0;
	std::uint16_t SocketNumber = 0;
	std::uint16_t ConnectionID = 0;
	std::uint16_t RestOfWorkspace = 0;
	std::uint8_t DriverWorkspace[12] = {};
	std::uint8_t ImmediateAddress[6] = {};
	std::uint16_t PacketCount = 0;
	struct {
		void* Address = nullptr;
		std::uint16_t Length = 0;
	} Packet[2];
};

inline void IPX_Copy_Address(NetNumType dst_net, NetNodeType dst_node, const NetNumType src_net, const NetNodeType src_node) {
	std::memcpy(dst_net, src_net, sizeof(NetNumType));
	std::memcpy(dst_node, src_node, sizeof(NetNodeType));
}

int IPX_SPX_Installed(void);
int IPX_Open_Socket(unsigned short socket);
int IPX_Close_Socket(unsigned short socket);
int IPX_Get_Connection_Number(void);
int IPX_Get_1st_Connection_Num(char* username);
int IPX_Get_Internet_Address(int connection_number, unsigned char* network_number, unsigned char* physical_node);
int IPX_Get_User_ID(int connection_number, char* user_id);
int IPX_Listen_For_Packet(ECBType* ecb_ptr);
void IPX_Send_Packet(ECBType* ecb_ptr);
int IPX_Get_Local_Target(unsigned char* dest_network, unsigned char* dest_node, unsigned short dest_socket, unsigned char* bridge_address);
int IPX_Cancel_Event(ECBType* ecb_ptr);
void Let_IPX_Breath(void);
