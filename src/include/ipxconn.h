#pragma once

#include "ipx.h"
#include "ipxaddr.h"

class IPXConnClass {
public:
	enum IPXConnTag {
		CONN_NAME_MAX = 40,
		CONNECTION_NONE = -1,
	};

	IPXConnClass(int, int, int, unsigned short, IPXAddressClass* address, int id, char* name);
	virtual ~IPXConnClass() = default;

	virtual void Init(void);

	static void Configure(unsigned short socket, int conn_num, ECBType* listen_ecb, ECBType* send_ecb, IPXHeaderType* listen_header,
	                      IPXHeaderType* send_header, char* listen_buf, char* send_buf, long handler_rm_ptr, int maxpacketlen);

	static bool Start_Listening(void);
	static bool Stop_Listening(void);

	virtual int Send(char* buf, int buflen);
	virtual int Receive_Packet(void*, int) { return 0; }
	virtual int Get_Packet(void*, int*) { return 0; }

	static int Open_Socket(unsigned short socket);
	static void Close_Socket(unsigned short socket);
	static int Send_To(char* buf, int buflen, IPXAddressClass* address, NetNodeType immed);
	static int Broadcast(char* buf, int buflen);

	IPXAddressClass Address{};
	NetNodeType ImmediateAddress{};
	int Immed_Set = 0;
	int ID = 0;
	char Name[CONN_NAME_MAX]{};

	static unsigned short Socket;
	static int ConnectionNum;
	static int PacketLen;
	static ECBType* ListenECB;
	static IPXHeaderType* ListenHeader;
	static char* ListenBuf;
	static ECBType* SendECB;
	static IPXHeaderType* SendHeader;
	static char* SendBuf;
	static long Handler;
	static int Configured;
	static int SocketOpen;
	static int Listening;
};
