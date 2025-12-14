#include "connect.h"
#include <chrono>

/* static command names - keep char* to match legacy header expectations */
char *ConnectionClass::Commands[PACKET_COUNT] = {
    (char *)"ADATA",
    (char *)"NDATA",
    (char *)"ACK"
};

ConnectionClass::ConnectionClass (int maxlen, unsigned short magicnum,
    unsigned long retry_delta, unsigned long max_retries, unsigned long timeout)
{
    MaxPacketLen = maxlen + sizeof(CommHeaderType);
    MagicNum = magicnum;
    RetryDelta = retry_delta;
    MaxRetries = max_retries;
    Timeout = timeout;

    PacketBuf = new char[ MaxPacketLen ];
}

ConnectionClass::~ConnectionClass ()
{
    delete [] PacketBuf;
}

int ConnectionClass::Service (void)
{
    if ( Service_Send_Queue() && Service_Receive_Queue() ) {
        return(1);
    } else {
        return(0);
    }
}

unsigned long ConnectionClass::Time (void)
{
    using namespace std::chrono;
    auto ms = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
    return static_cast<unsigned long>((ms * 6) / 100);
}

char *ConnectionClass::Command_Name(int command)
{
    if (command >= 0 && command < PACKET_COUNT) {
        return(Commands[command]);
    } else {
        return(nullptr);
    }
}
