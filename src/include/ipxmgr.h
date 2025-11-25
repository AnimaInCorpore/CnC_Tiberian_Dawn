#pragma once

#include "ipxgconn.h"

class IPXManagerClass {
public:
	static constexpr int CONNECT_MAX = 6;

	IPXManagerClass(int glb_maxlen, int pvt_maxlen, int glb_num_packets, int pvt_num_packets, unsigned short socket, unsigned short product_id);
	virtual ~IPXManagerClass();

	virtual void Init(void);
	int Is_IPX(void) const;
	virtual void Set_Timing(unsigned long retrydelta, unsigned long maxretries, unsigned long timeout);
	bool Set_Bridge(IPXAddressClass* address);

	virtual bool Create_Connection(int id, char* name, IPXAddressClass* address);
	virtual bool Delete_Connection(int id);
	virtual int Num_Connections(void);
	virtual int Connection_ID(int index);
	virtual char* Connection_Name(int id);
	virtual IPXAddressClass* Connection_Address(int id);
	virtual int Connection_Index(int id);

	int Send_Global_Message(void* buf, int buflen, IPXAddressClass* address, int ack_req);
	int Get_Global_Message(void* buf, int* buflen, IPXAddressClass* address, unsigned short* product_id);

	virtual int Send_Private_Message(void* buf, int buflen, int ack_req = 1, int conn_id = IPXConnClass::CONNECTION_NONE);
	virtual int Get_Private_Message(void* buf, int* buflen, int* conn_id);

	virtual int Service(void);
	int Get_Bad_Connection(void);

	virtual int Global_Num_Send(void);
	virtual int Global_Num_Receive(void);
	virtual int Private_Num_Send(int id = IPXConnClass::CONNECTION_NONE);
	virtual int Private_Num_Receive(int id = IPXConnClass::CONNECTION_NONE);

	void Set_Socket(unsigned short socket);

	virtual unsigned long Response_Time(void);
	unsigned long Global_Response_Time(void);
	virtual void Reset_Response_Time(void);

	virtual void* Oldest_Send(void);

	virtual void Configure_Debug(int index, int offset, int size, char** names, int maxnames);
	virtual void Mono_Debug_Print(int index, int refresh = 0);

protected:
	int Match_Connection(IPXAddressClass const& address) const;
	void Drain_Incoming();
	IPXGlobalConnClass* GlobalChannel;
	IPXConnClass* Connection[CONNECT_MAX];
	int IPXStatus;
	unsigned short Socket;
	int NumConnections;
	int BadConnection;
	int RetryDelta;
	int MaxRetries;
	int Timeout;
	unsigned short ProductID;
};
