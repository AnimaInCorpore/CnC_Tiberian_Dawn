#include "ipx.h"

#include <cstring>

int IPX_SPX_Installed(void) {
	// Always report availability so higher-level logic can continue to run.
	return 1;
}

int IPX_Open_Socket(unsigned short) { return 0; }

int IPX_Close_Socket(unsigned short) { return 0; }

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
	if (ecb_ptr) {
		ecb_ptr->InUse = 0;
		ecb_ptr->CompletionCode = 0;
	}
	return 0;
}

void IPX_Send_Packet(ECBType* ecb_ptr) {
	if (ecb_ptr) {
		ecb_ptr->InUse = 0;
		ecb_ptr->CompletionCode = 0;
	}
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

void Let_IPX_Breath(void) {}
