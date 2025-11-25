#include "ipxgconn.h"

IPXGlobalConnClass::IPXGlobalConnClass(int numsend, int numrecieve, int maxlen, unsigned short product_id)
    : IPXConnClass(numsend, numrecieve, maxlen, GLOBAL_MAGICNUM, nullptr, 0, nullptr), ProductID(product_id) {}

int IPXGlobalConnClass::Send_Packet(void* buf, int buflen, IPXAddressClass* address, int) {
	return IPXConnClass::Send_To(static_cast<char*>(buf), buflen, address, ImmediateAddress);
}

int IPXGlobalConnClass::Receive_Packet(void* buf, int buflen, IPXAddressClass* address) {
	return IPXConnClass::Pop_Packet(static_cast<char*>(buf), buflen, address);
}

int IPXGlobalConnClass::Get_Packet(void* buf, int* buflen, IPXAddressClass* address, unsigned short* product_id) {
	if (!buf || !buflen) return 0;
	if (product_id) *product_id = ProductID;
	const int received = Receive_Packet(buf, *buflen, address);
	*buflen = received;
	return received;
}
