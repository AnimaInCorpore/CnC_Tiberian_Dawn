/*
**	Command & Conquer(tm)
**	Ported communications buffer class (partial)
*/

#include "legacy/function.h"
#include "legacy/combuf.h"

CommBufferClass::CommBufferClass(int numsend, int numreceive, int maxlen)
{
    int i;

    MaxSend = numsend;
    MaxReceive = numreceive;
    MaxPacketSize = maxlen;

    SendQueue = new SendQueueType[numsend];
    ReceiveQueue = new ReceiveQueueType[numreceive];

    SendIndex = new int[numsend];
    ReceiveIndex = new int[numreceive];

    for (i = 0; i < MaxSend; i++) {
        SendQueue[i].Buffer = new char[maxlen];
    }

    for (i = 0; i < MaxReceive; i++) {
        ReceiveQueue[i].Buffer = new char[maxlen];
    }

    Init();
}

CommBufferClass::~CommBufferClass()
{
    int i;
    for (i = 0; i < MaxSend; i++) {
        delete [] SendQueue[i].Buffer;
    }
    for (i = 0; i < MaxReceive; i++) {
        delete [] ReceiveQueue[i].Buffer;
    }
    delete [] SendQueue;
    delete [] ReceiveQueue;
    delete [] SendIndex;
    delete [] ReceiveIndex;
}

void CommBufferClass::Init(void)
{
    int i;
    SendTotal = 0L;
    ReceiveTotal = 0L;
    DelaySum = 0L;
    NumDelay = 0L;
    MeanDelay = 0L;
    MaxDelay = 0L;
    SendCount = 0;
    ReceiveCount = 0;

    for (i = 0; i < MaxSend; i++) {
        SendQueue[i].IsActive = 0;
        SendQueue[i].IsACK = 0;
        SendQueue[i].FirstTime = 0L;
        SendQueue[i].LastTime = 0L;
        SendQueue[i].SendCount = 0L;
        SendQueue[i].BufLen = 0;
        SendIndex[i] = 0;
    }

    for (i = 0; i < MaxReceive; i++) {
        ReceiveQueue[i].IsActive = 0;
        ReceiveQueue[i].IsRead = 0;
        ReceiveQueue[i].IsACK = 0;
        ReceiveQueue[i].BufLen = 0;
        ReceiveIndex[i] = 0;
    }

    DebugOffset = 0;
    DebugSize = 0;
    DebugNames = nullptr;
    DebugMaxNames = 0;
}

void CommBufferClass::Init_Send_Queue(void)
{
    int i;
    SendCount = 0;
    for (i = 0; i < MaxSend; i++) {
        SendQueue[i].IsActive = 0;
        SendQueue[i].IsACK = 0;
        SendQueue[i].FirstTime = 0L;
        SendQueue[i].LastTime = 0L;
        SendQueue[i].SendCount = 0L;
        SendIndex[i] = 0;
    }
}

int CommBufferClass::Queue_Send(void *buf, int buflen)
{
    int i;
    int index = -1;
    if (SendCount == MaxSend) return 0;
    for (i = 0; i < MaxSend; i++) {
        if (SendQueue[i].IsActive == 0) { index = i; break; }
    }
    if (index == -1) return 0;

    SendQueue[index].IsActive = 1;
    SendQueue[index].IsACK = 0;
    SendQueue[index].FirstTime = 0L;
    SendQueue[index].LastTime = 0L;
    SendQueue[index].SendCount = 0L;
    SendQueue[index].BufLen = buflen;
    memcpy(SendQueue[index].Buffer, buf, buflen);
    SendIndex[SendCount] = index;
    SendCount++;
    SendTotal++;
    return 1;
}

int CommBufferClass::UnQueue_Send(void *buf, int *buflen, int index)
{
    int i;
    if (SendCount == 0 || SendQueue[SendIndex[index]].IsActive == 0) return 0;
    if (buf != NULL) {
        memcpy(buf, SendQueue[SendIndex[index]].Buffer, SendQueue[SendIndex[index]].BufLen);
        (*buflen) = SendQueue[SendIndex[index]].BufLen;
    }
    SendQueue[SendIndex[index]].IsActive = 0;
    SendQueue[SendIndex[index]].IsACK = 0;
    SendQueue[SendIndex[index]].FirstTime = 0L;
    SendQueue[SendIndex[index]].LastTime = 0L;
    SendQueue[SendIndex[index]].SendCount = 0L;
    SendQueue[SendIndex[index]].BufLen = 0;
    for (i = index; i < SendCount - 1; i++) {
        SendIndex[i] = SendIndex[i + 1];
    }
    SendIndex[SendCount - 1] = 0;
    SendCount--;
    return 1;
}

SendQueueType * CommBufferClass::Get_Send(int index)
{
    if (SendQueue[SendIndex[index]].IsActive == 0) return nullptr;
    return &SendQueue[SendIndex[index]];
}

int CommBufferClass::Queue_Receive(void *buf, int buflen)
{
    int i, index = -1;
    if (ReceiveCount == MaxReceive) return 0;
    for (i = 0; i < MaxReceive; i++) {
        if (ReceiveQueue[i].IsActive == 0) { index = i; break; }
    }
    if (index == -1) return 0;
    ReceiveQueue[index].IsActive = 1;
    ReceiveQueue[index].IsRead = 0;
    ReceiveQueue[index].IsACK = 0;
    ReceiveQueue[index].BufLen = buflen;
    memcpy(ReceiveQueue[index].Buffer, buf, buflen);
    ReceiveIndex[ReceiveCount] = index;
    ReceiveCount++;
    ReceiveTotal++;
    return 1;
}

int CommBufferClass::UnQueue_Receive(void *buf, int *buflen, int index)
{
    int i;
    if (ReceiveCount == 0 || ReceiveQueue[ReceiveIndex[index]].IsActive == 0) return 0;
    if (buf != NULL) {
        memcpy(buf, ReceiveQueue[ReceiveIndex[index]].Buffer, ReceiveQueue[ReceiveIndex[index]].BufLen);
        (*buflen) = ReceiveQueue[ReceiveIndex[index]].BufLen;
    }
    ReceiveQueue[ReceiveIndex[index]].IsActive = 0;
    ReceiveQueue[ReceiveIndex[index]].IsRead = 0;
    ReceiveQueue[ReceiveIndex[index]].IsACK = 0;
    ReceiveQueue[ReceiveIndex[index]].BufLen = 0;
    for (i = index; i < ReceiveCount - 1; i++) {
        ReceiveIndex[i] = ReceiveIndex[i + 1];
    }
    ReceiveIndex[ReceiveCount - 1] = 0;
    ReceiveCount--;
    return 1;
}

ReceiveQueueType * CommBufferClass::Get_Receive(int index)
{
    if (ReceiveQueue[ReceiveIndex[index]].IsActive == 0) return nullptr;
    return &ReceiveQueue[ReceiveIndex[index]];
}

void CommBufferClass::Add_Delay(unsigned long delay)
{
    int roundoff = 0;
    if (NumDelay == 256) {
        DelaySum -= MeanDelay;
        DelaySum += delay;
        if ((DelaySum & 0x00ff) > 127) roundoff = 1;
        MeanDelay = (DelaySum >> 8) + roundoff;
    } else {
        NumDelay++;
        DelaySum += delay;
        MeanDelay = DelaySum / NumDelay;
    }
    if (delay > MaxDelay) MaxDelay = delay;
}

unsigned long CommBufferClass::Avg_Response_Time(void)
{
    return MeanDelay;
}

unsigned long CommBufferClass::Max_Response_Time(void)
{
    return MaxDelay;
}

void CommBufferClass::Reset_Response_Time(void)
{
    DelaySum = 0L;
    NumDelay = 0L;
    MeanDelay = 0L;
    MaxDelay = 0L;
}

void CommBufferClass::Configure_Debug(int offset, int size, char **names, int maxnames)
{
    DebugOffset = offset;
    DebugSize = size;
    DebugNames = names;
    DebugMaxNames = maxnames;
}

void CommBufferClass::Mono_Debug_Print(int refresh)
{
#ifdef WWLIB32_H
    Mono_Debug_Print2(refresh);
#endif
}

void CommBufferClass::Mono_Debug_Print2(int) { /* stubbed for non-debug builds */ }
