#pragma once

#include <cstdint>

extern "C" {
bool IPX_Initialise(void);
bool IPX_Get_Outstanding_Buffer95(unsigned char* buffer);
void IPX_Shut_Down95(void);
int IPX_Send_Packet95(unsigned char* send_address, unsigned char* buf, int buflen, unsigned char* net, unsigned char* node);
int IPX_Broadcast_Packet95(unsigned char* buf, int buflen);
bool IPX_Start_Listening95(void);
int IPX_Open_Socket95(int socket);
void IPX_Close_Socket95(int socket);
int IPX_Get_Connection_Number95(void);
int IPX_Get_Local_Target95(unsigned char* dest_network, unsigned char* dest_node, unsigned short dest_socket, unsigned char* bridge_address);
}
