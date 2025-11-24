#include "ipxmgr.h"
#include "ipx95.h"

#include <memory>

IPXManagerClass::IPXManagerClass(int glb_maxlen, int pvt_maxlen, int glb_num_packets, int pvt_num_packets, unsigned short socket, unsigned short product_id)
    : GlobalChannel(nullptr), IPXStatus(0), Socket(socket) {
	(void)glb_maxlen;
	(void)pvt_maxlen;
	(void)glb_num_packets;
	(void)pvt_num_packets;
	if (IPX_SPX_Installed()) {
		IPXStatus = 1;
		GlobalChannel = new IPXGlobalConnClass(0, 0, 0, product_id);
	}
}

IPXManagerClass::~IPXManagerClass() { delete GlobalChannel; }

void IPXManagerClass::Init(void) {
	if (GlobalChannel) GlobalChannel->Init();
}

bool IPXManagerClass::Set_Bridge(IPXAddressClass* address) {
	(void)address;
	return true;
}

void IPXManagerClass::Set_Socket(unsigned short socket) { Socket = socket; }

int IPXManagerClass::Send_Global_Message(void* buf, int buflen, IPXAddressClass* address, int ack_req) {
	if (!GlobalChannel) return 0;
	return GlobalChannel->Send_Packet(buf, buflen, address, ack_req);
}

int IPXManagerClass::Get_Global_Message(void* buf, int* buflen, IPXAddressClass* address, unsigned short* product_id) {
	if (!GlobalChannel) return 0;
	return GlobalChannel->Get_Packet(buf, buflen, address, product_id);
}
