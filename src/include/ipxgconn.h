#pragma once

#include "ipxconn.h"

struct IPXCommHeaderType {
	unsigned short Magic = 0;
	unsigned short PacketID = 0;
};

struct GlobalHeaderType {
	IPXCommHeaderType Header{};
	IPXAddressClass Address{};
	unsigned short ProductID = 0;
};

class IPXGlobalConnClass : public IPXConnClass {
public:
	enum GlobalConnectionEnum {
		GLOBAL_MAGICNUM = 0x1234,
		COMMAND_AND_CONQUER = 0xaa01,
	};

	IPXGlobalConnClass(int numsend, int numrecieve, int maxlen, unsigned short product_id);
	~IPXGlobalConnClass() override = default;

	int Send_Packet(void* buf, int buflen, IPXAddressClass* address, int ack_req);
	int Receive_Packet(void* buf, int buflen, IPXAddressClass* address);
	int Get_Packet(void* buf, int* buflen, IPXAddressClass* address, unsigned short* product_id);

	unsigned short ProductID = 0;
};
