#ifndef CONNECTION_H
#define CONNECTION_H

#define CONN_DEBUG 0

typedef struct {
    unsigned short MagicNumber;
    unsigned char Code;
    unsigned long PacketID;
} CommHeaderType;

class ConnectionClass {
public:
    enum ConnectionEnum {
        PACKET_DATA_ACK,
        PACKET_DATA_NOACK,
        PACKET_ACK,
        PACKET_COUNT
    };

    ConnectionClass(int maxlen, unsigned short magicnum, unsigned long retry_delta, unsigned long max_retries, unsigned long timeout);
    virtual ~ConnectionClass();

    virtual void Init(void) {}

    virtual int Send_Packet(void* buf, int buflen, int ack_req) = 0;
    virtual int Receive_Packet(void* buf, int buflen) = 0;
    virtual int Get_Packet(void* buf, int* buflen) = 0;

    virtual int Service(void);

    static unsigned long Time(void);

    unsigned short Magic_Num(void) { return MagicNum; }
    unsigned long Retry_Delta(void) { return RetryDelta; }
    void Set_Retry_Delta(unsigned long delta) { RetryDelta = delta; }
    unsigned long Max_Retries(void) { return MaxRetries; }
    void Set_Max_Retries(unsigned long retries) { MaxRetries = retries; }
    unsigned long Time_Out(void) { return Timeout; }
    void Set_TimeOut(unsigned long t) { Timeout = t; }
    unsigned long Max_Packet_Len(void) { return MaxPacketLen; }
    static const char* Command_Name(int command);

protected:
    virtual int Service_Send_Queue(void) = 0;
    virtual int Service_Receive_Queue(void) = 0;
    virtual int Send(char* buf, int buflen) = 0;

    int MaxPacketLen;
    char* PacketBuf;
    unsigned short MagicNum;
    unsigned long RetryDelta;
    unsigned long MaxRetries;
    unsigned long Timeout;

    static const char* Commands[PACKET_COUNT];
};

#endif
