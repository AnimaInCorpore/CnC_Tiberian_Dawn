#include "ipx95.h"
#include "ipx.h"

bool IPX_Initialise(void) { return true; }

bool IPX_Get_Outstanding_Buffer95(unsigned char*) { return false; }

void IPX_Shut_Down95(void) {}

int IPX_Send_Packet95(unsigned char*, unsigned char*, int buflen, unsigned char*, unsigned char*) { return buflen; }

int IPX_Broadcast_Packet95(unsigned char*, int buflen) { return buflen; }

bool IPX_Start_Listening95(void) { return true; }

int IPX_Open_Socket95(int socket) { return IPX_Open_Socket(static_cast<unsigned short>(socket)); }

void IPX_Close_Socket95(int socket) { IPX_Close_Socket(static_cast<unsigned short>(socket)); }

int IPX_Get_Connection_Number95(void) { return IPX_Get_Connection_Number(); }

int IPX_Get_Local_Target95(unsigned char* dest_network, unsigned char* dest_node, unsigned short dest_socket, unsigned char* bridge_address) {
	return IPX_Get_Local_Target(dest_network, dest_node, dest_socket, bridge_address);
}
