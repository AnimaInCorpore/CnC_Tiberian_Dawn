#pragma once

#include "ipxgconn.h"

class IPXManagerClass {
public:
	IPXManagerClass(int glb_maxlen, int pvt_maxlen, int glb_num_packets, int pvt_num_packets, unsigned short socket, unsigned short product_id);
	virtual ~IPXManagerClass();

	virtual void Init(void);
	bool Is_IPX(void) const { return IPXStatus != 0; }
	bool Set_Bridge(IPXAddressClass* address);
	void Set_Socket(unsigned short socket);

	int Send_Global_Message(void* buf, int buflen, IPXAddressClass* address, int ack_req);
	int Get_Global_Message(void* buf, int* buflen, IPXAddressClass* address, unsigned short* product_id);

	int Global_Num_Send(void) const { return 0; }
	int Global_Num_Receive(void) const { return 0; }

protected:
	IPXGlobalConnClass* GlobalChannel;
	int IPXStatus;
	unsigned short Socket;
};
