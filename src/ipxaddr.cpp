#include "ipxaddr.h"

#include <cstring>

IPXAddressClass::IPXAddressClass() : NetworkNumber{0}, NodeAddress{0} {}

IPXAddressClass::IPXAddressClass(NetNumType net, NetNodeType node) : NetworkNumber{0}, NodeAddress{0} {
	Set_Address(net, node);
}

IPXAddressClass::IPXAddressClass(IPXHeaderType* header) : NetworkNumber{0}, NodeAddress{0} {
	Set_Address(header);
}

void IPXAddressClass::Set_Address(NetNumType net, NetNodeType node) {
	std::memcpy(NetworkNumber, net, sizeof(NetworkNumber));
	std::memcpy(NodeAddress, node, sizeof(NodeAddress));
}

void IPXAddressClass::Set_Address(IPXHeaderType* header) {
	if (!header) return;
	Set_Address(header->SourceNetworkNumber, header->SourceNetworkNode);
}

void IPXAddressClass::Get_Address(NetNumType net, NetNodeType node) {
	std::memcpy(net, NetworkNumber, sizeof(NetworkNumber));
	std::memcpy(node, NodeAddress, sizeof(NodeAddress));
}

void IPXAddressClass::Get_Address(IPXHeaderType* header) {
	if (!header) return;
	std::memcpy(header->DestNetworkNumber, NetworkNumber, sizeof(NetworkNumber));
	std::memcpy(header->DestNetworkNode, NodeAddress, sizeof(NodeAddress));
}

bool IPXAddressClass::Is_Broadcast(void) {
	for (unsigned char value : NetworkNumber) {
		if (value != 0xFF) return false;
	}
	for (unsigned char value : NodeAddress) {
		if (value != 0xFF) return false;
	}
	return true;
}

int IPXAddressClass::operator==(IPXAddressClass& addr) {
	return std::memcmp(NetworkNumber, addr.NetworkNumber, sizeof(NetworkNumber)) == 0 &&
	       std::memcmp(NodeAddress, addr.NodeAddress, sizeof(NodeAddress)) == 0;
}

int IPXAddressClass::operator!=(IPXAddressClass& addr) { return !(*this == addr); }

int IPXAddressClass::operator>(IPXAddressClass& addr) {
	const int cmp = std::memcmp(NetworkNumber, addr.NetworkNumber, sizeof(NetworkNumber));
	if (cmp != 0) return cmp > 0;
	return std::memcmp(NodeAddress, addr.NodeAddress, sizeof(NodeAddress)) > 0;
}

int IPXAddressClass::operator<(IPXAddressClass& addr) { return addr > *this; }

int IPXAddressClass::operator>=(IPXAddressClass& addr) { return !(*this < addr); }

int IPXAddressClass::operator<=(IPXAddressClass& addr) { return !(*this > addr); }
