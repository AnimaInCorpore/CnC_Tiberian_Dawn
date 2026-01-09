#include "connect.h"

#if defined(_WIN32)
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif

const char* ConnectionClass::Commands[PACKET_COUNT] = {"ADATA", "NDATA", "ACK"};

ConnectionClass::ConnectionClass(int maxlen, unsigned short magicnum, unsigned long retry_delta, unsigned long max_retries,
                                 unsigned long timeout)
{
    MaxPacketLen = maxlen + (int)sizeof(CommHeaderType);
    MagicNum = magicnum;
    RetryDelta = retry_delta;
    MaxRetries = max_retries;
    Timeout = timeout;
    PacketBuf = new char[MaxPacketLen];
}

ConnectionClass::~ConnectionClass() { delete[] PacketBuf; }

int ConnectionClass::Service(void)
{
    if (Service_Send_Queue() && Service_Receive_Queue()) {
        return 1;
    }
    return 0;
}

unsigned long ConnectionClass::Time(void)
{
#if defined(_WIN32)
    struct _timeb mytime;
    _ftime(&mytime);
    unsigned long msec = (unsigned long)mytime.time * 1000UL + (unsigned long)mytime.millitm;
#else
    struct timeval tv;
    gettimeofday(&tv, 0);
    unsigned long msec = (unsigned long)tv.tv_sec * 1000UL + (unsigned long)(tv.tv_usec / 1000UL);
#endif
    return (msec / 100UL) * 6UL;
}

const char* ConnectionClass::Command_Name(int command)
{
    if (command >= 0 && command < PACKET_COUNT) {
        return Commands[command];
    }
    return 0;
}
