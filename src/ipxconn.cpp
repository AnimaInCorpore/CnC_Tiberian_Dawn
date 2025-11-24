#include "ipxconn.h"
#include "ipx95.h"

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

IPXConnClass::IPXConnClass(int numsend, int numreceive, int maxlen, unsigned short magicnum, IPXAddressClass* address, int id, char* name)
    : NonSequencedConnClass(numsend, numreceive, maxlen, magicnum, 0, 0, 0) {
	if (address) {
		Address = *address;
	}
	ID = id;
	if (name) {
		std::strncpy(Name, name, sizeof(Name) - 1);
		Name[sizeof(Name) - 1] = '\0';
	}
}

void IPXConnClass::Init(void) { Immed_Set = 0; }

void IPXConnClass::Configure(unsigned short socket, int conn_num, ECBType* listen_ecb, ECBType* send_ecb, IPXHeaderType* listen_header,
                             IPXHeaderType* send_header, char* listen_buf, char* send_buf, long handler_rm_ptr, int maxpacketlen) {
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
	return IPX_Start_Listening95();
}

bool IPXConnClass::Stop_Listening(void) {
	Listening = 0;
	return true;
}

int IPXConnClass::Send(char* buf, int buflen) {
	if (!buf || buflen <= 0) return 0;
	return IPX_Send_Packet95(reinterpret_cast<unsigned char*>(ImmediateAddress), reinterpret_cast<unsigned char*>(buf), buflen, nullptr, nullptr);
}

int IPXConnClass::Open_Socket(unsigned short socket) { return IPX_Open_Socket95(static_cast<int>(socket)); }

void IPXConnClass::Close_Socket(unsigned short socket) { IPX_Close_Socket95(static_cast<int>(socket)); }

int IPXConnClass::Send_To(char* buf, int buflen, IPXAddressClass* address, NetNodeType immed) {
	if (address) {
		address->Get_Address(SendHeader ? SendHeader->DestNetworkNumber : nullptr, immed);
	}
	return IPX_Send_Packet95(immed, reinterpret_cast<unsigned char*>(buf), buflen, nullptr, nullptr);
}

int IPXConnClass::Broadcast(char* buf, int buflen) { return IPX_Broadcast_Packet95(reinterpret_cast<unsigned char*>(buf), buflen); }
