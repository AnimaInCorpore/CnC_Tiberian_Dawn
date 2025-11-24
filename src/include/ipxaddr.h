#pragma once

#include "ipx.h"

class IPXAddressClass {
public:
	NetNumType NetworkNumber{};
	NetNodeType NodeAddress{};

	IPXAddressClass();
	IPXAddressClass(NetNumType net, NetNodeType node);
	IPXAddressClass(IPXHeaderType* header);

	void Set_Address(NetNumType net, NetNodeType node);
	void Set_Address(IPXHeaderType* header);
	void Get_Address(NetNumType net, NetNodeType node);
	void Get_Address(IPXHeaderType* header);
	bool Is_Broadcast(void);

	int operator==(IPXAddressClass& addr);
	int operator!=(IPXAddressClass& addr);
	int operator>(IPXAddressClass& addr);
	int operator<(IPXAddressClass& addr);
	int operator>=(IPXAddressClass& addr);
	int operator<=(IPXAddressClass& addr);
};
