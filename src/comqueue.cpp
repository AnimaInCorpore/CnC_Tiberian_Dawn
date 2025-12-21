/*
**	Command & Conquer(tm)
**	Ported comm queue class (partial)
*/

#include "legacy/function.h"
#include "legacy/comqueue.h"

CommQueueClass::CommQueueClass(int numsend, int numreceive, int maxlen)
{
    int i;
    MaxSend = numsend;
    MaxReceive = numreceive;
    MaxPacketSize = maxlen;

    SendQueue = new SendQueueType[numsend];
    ReceiveQueue = new ReceiveQueueType[numreceive];

    for (i = 0; i < MaxSend; i++) {
        SendQueue[i].Buffer = new char[maxlen];
    }
    for (i = 0; i < MaxReceive; i++) {
        ReceiveQueue[i].Buffer = new char[maxlen];
    }
    Init();
}

CommQueueClass::~CommQueueClass()
{
    int i;
    for (i = 0; i < MaxSend; i++) delete [] SendQueue[i].Buffer;
    for (i = 0; i < MaxReceive; i++) delete [] ReceiveQueue[i].Buffer;
    delete [] SendQueue;
    delete [] ReceiveQueue;
}

void CommQueueClass::Init(void)
{
    int i;
    SendTotal = 0L;
    ReceiveTotal = 0L;
    DelaySum = 0L;
    NumDelay = 0L;
    MeanDelay = 0L;
    MaxDelay = 0L;
    SendCount = 0; SendNext = 0; SendEmpty = 0;
    ReceiveCount = 0; ReceiveNext = 0; ReceiveEmpty = 0;
    for (i = 0; i < MaxSend; i++) {
        SendQueue[i].IsActive = 0;
        SendQueue[i].IsACK = 0;
        SendQueue[i].FirstTime = 0L;
        SendQueue[i].LastTime = 0L;
        SendQueue[i].SendCount = 0L;
        SendQueue[i].BufLen = 0;
    }
    for (i = 0; i < MaxReceive; i++) {
        ReceiveQueue[i].IsActive = 0;
        ReceiveQueue[i].IsRead = 0;
        ReceiveQueue[i].IsACK = 0;
        ReceiveQueue[i].BufLen = 0;
    }
    DebugOffset = 0; DebugSize = 0; DebugNames = nullptr; DebugMaxNames = 0;
}

int CommQueueClass::Queue_Send(void *buf, int buflen)
{
    if (SendCount == MaxSend || SendQueue[SendEmpty].IsActive != 0) return 0;
    SendQueue[SendEmpty].IsActive = 1;
    SendQueue[SendEmpty].IsACK = 0;
    SendQueue[SendEmpty].FirstTime = 0L;
    SendQueue[SendEmpty].LastTime = 0L;
    SendQueue[SendEmpty].SendCount = 0L;
    SendQueue[SendEmpty].BufLen = buflen;
    memcpy(SendQueue[SendEmpty].Buffer, buf, buflen);
    SendCount++; SendEmpty++; if (SendEmpty == MaxSend) SendEmpty = 0; SendTotal++;
    return 1;
}

int CommQueueClass::UnQueue_Send(void *buf, int *buflen)
{
    if (SendCount == 0 || SendQueue[SendNext].IsActive == 0) return 0;
    if (buf != NULL) {
        memcpy(buf, SendQueue[SendNext].Buffer, SendQueue[SendNext].BufLen);
        (*buflen) = SendQueue[SendNext].BufLen;
    }
    SendQueue[SendNext].IsActive = 0;
    SendQueue[SendNext].IsACK = 0;
    SendQueue[SendNext].FirstTime = 0L;
    SendQueue[SendNext].LastTime = 0L;
    SendQueue[SendNext].SendCount = 0L;
    SendQueue[SendNext].BufLen = 0;
    SendCount--; SendNext++; if (SendNext == MaxSend) SendNext = 0;
    return 1;
}

SendQueueType * CommQueueClass::Next_Send(void)
{
    if (SendCount == 0) return nullptr;
    return &SendQueue[SendNext];
}

SendQueueType * CommQueueClass::Get_Send(int index)
{
    int i = SendNext + index;
    if (i >= MaxSend) i -= MaxSend;
    if (SendQueue[i].IsActive == 0) return nullptr;
    return &SendQueue[i];
}

int CommQueueClass::Queue_Receive(void *buf, int buflen)
{
    if (ReceiveCount == MaxReceive || ReceiveQueue[ReceiveEmpty].IsActive != 0) return 0;
    ReceiveQueue[ReceiveEmpty].IsActive = 1;
    ReceiveQueue[ReceiveEmpty].IsRead = 0;
    ReceiveQueue[ReceiveEmpty].IsACK = 0;
    ReceiveQueue[ReceiveEmpty].BufLen = buflen;
    memcpy(ReceiveQueue[ReceiveEmpty].Buffer, buf, buflen);
    ReceiveCount++; ReceiveEmpty++; if (ReceiveEmpty == MaxReceive) ReceiveEmpty = 0; ReceiveTotal++;
    return 1;
}

int CommQueueClass::UnQueue_Receive(void *buf, int *buflen)
{
    if (ReceiveCount == 0 || ReceiveQueue[ReceiveNext].IsActive == 0) return 0;
    if (buf != NULL) {
        memcpy(buf, ReceiveQueue[ReceiveNext].Buffer, ReceiveQueue[ReceiveNext].BufLen);
        (*buflen) = ReceiveQueue[ReceiveNext].BufLen;
    }
    ReceiveQueue[ReceiveNext].IsActive = 0;
    ReceiveQueue[ReceiveNext].IsRead = 0;
    ReceiveQueue[ReceiveNext].IsACK = 0;
    ReceiveQueue[ReceiveNext].BufLen = 0;
    ReceiveCount--; ReceiveNext++; if (ReceiveNext == MaxReceive) ReceiveNext = 0;
    return 1;
}

ReceiveQueueType * CommQueueClass::Next_Receive(void)
{
    if (ReceiveCount == 0) return nullptr;
    return &ReceiveQueue[ReceiveNext];
}

ReceiveQueueType * CommQueueClass::Get_Receive(int index)
{
    int i = ReceiveNext + index;
    if (i >= MaxReceive) i -= MaxReceive;
    if (ReceiveQueue[i].IsActive == 0) return nullptr;
    return &ReceiveQueue[i];
}

void CommQueueClass::Add_Delay(unsigned long delay)
{
    int roundoff = 0;
    if (NumDelay == 256) {
        DelaySum -= MeanDelay; DelaySum += delay;
        if ((DelaySum & 0x00ff) > 127) roundoff = 1;
        MeanDelay = (DelaySum >> 8) + roundoff;
    } else {
        NumDelay++; DelaySum += delay; MeanDelay = DelaySum / NumDelay;
    }
    if (delay > MaxDelay) MaxDelay = delay;
}

unsigned long CommQueueClass::Avg_Response_Time(void) { return MeanDelay; }
unsigned long CommQueueClass::Max_Response_Time(void) { return MaxDelay; }
void CommQueueClass::Reset_Response_Time(void) { DelaySum = 0L; NumDelay = 0L; MeanDelay = 0L; MaxDelay = 0L; }

void CommQueueClass::Configure_Debug(int offset, int size, char **names, int maxnames) {
    DebugOffset = offset; DebugSize = size; DebugNames = names; DebugMaxNames = maxnames;
}

void CommQueueClass::Mono_Debug_Print(int refresh) {
#ifdef WWLIB32_H
    Mono_Debug_Print2(refresh);
#endif
}

void CommQueueClass::Mono_Debug_Print2(int refresh) {
#ifdef WWLIB32_H
  int i;
  char txt[80];
  int val = 0;

  struct CommHdr {
    unsigned short MagicNumber;
    unsigned char Code;
    unsigned long PacketID;
  } * hdr;

  if (refresh) {
    Mono_Clear_Screen();
    Mono_Printf("ﾚﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄｿ\n");
    Mono_Printf("ｳ                                                                             ｳ\n");
    Mono_Printf("ｳ                                                                             ｳ\n");
    Mono_Printf("ｳ                                                                             ｳ\n");
    Mono_Printf("ﾃﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾂﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄｴ\n");
    Mono_Printf("ｳ              Send Queue              ｳ             Receive Queue            ｳ\n");
    Mono_Printf("ｳ                                      ｳ                                      ｳ\n");
    Mono_Printf("ｳ ID  Ct Type   Data  Name         ACK ｳ ID  Rd Type   Data  Name         ACK ｳ\n");
    Mono_Printf("ｳ                                      ｳ                                      ｳ\n");
    Mono_Printf("ｳ                                      ｳ                                      ｳ\n");
    Mono_Printf("ｳ                                      ｳ                                      ｳ\n");
    Mono_Printf("ｳ                                      ｳ                                      ｳ\n");
    Mono_Printf("ｳ                                      ｳ                                      ｳ\n");
    Mono_Printf("ｳ                                      ｳ                                      ｳ\n");
    Mono_Printf("ｳ                                      ｳ                                      ｳ\n");
    Mono_Printf("ｳ                                      ｳ                                      ｳ\n");
    Mono_Printf("ｳ                                      ｳ                                      ｳ\n");
    Mono_Printf("ｳ                                      ｳ                                      ｳ\n");
    Mono_Printf("ｳ                                      ｳ                                      ｳ\n");
    Mono_Printf("ｳ                                      ｳ                                      ｳ\n");
    Mono_Printf("ｳ                                      ｳ                                      ｳ\n");
    Mono_Printf("ｳ                                      ｳ                                      ｳ\n");
    Mono_Printf("ｳ                                      ｳ                                      ｳ\n");
    Mono_Printf("ｳ                                      ｳ                                      ｳ\n");
    Mono_Printf("ﾀﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾁﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾄﾙ");
  }

  for (i = 0; i < MaxSend; i++) {
    Mono_Set_Cursor(1, 8 + i);
    if (SendQueue[i].IsActive) {
      hdr = reinterpret_cast<CommHdr*>(SendQueue[i].Buffer);
      hdr->MagicNumber = hdr->MagicNumber;
      hdr->Code = hdr->Code;
      std::sprintf(txt, "%4d %2d %-5s  ", static_cast<int>(hdr->PacketID),
                   SendQueue[i].SendCount,
                   ConnectionClass::Command_Name(hdr->Code));

      if (DebugSize && (DebugOffset + DebugSize) <= SendQueue[i].BufLen) {
        if (DebugSize == 1) {
          val = *(SendQueue[i].Buffer + DebugOffset);
        } else if (DebugSize == 2) {
          val = *((short*)(SendQueue[i].Buffer + DebugOffset));
        } else if (DebugSize == 4) {
          val = *((int*)(SendQueue[i].Buffer + DebugOffset));
        }
        std::sprintf(txt + std::strlen(txt), "%4d  ", val);

        if (DebugMaxNames && val > 0 && val < DebugMaxNames) {
          std::sprintf(txt + std::strlen(txt), "%-12s  %x", DebugNames[val],
                       SendQueue[i].IsACK);
        } else {
          std::sprintf(txt + std::strlen(txt), "              %x",
                       SendQueue[i].IsACK);
        }
      }
      Mono_Printf(txt);
    } else {
      Mono_Printf("____ __                            _");
    }
  }

  for (i = 0; i < MaxReceive; i++) {
    Mono_Set_Cursor(40, 8 + i);
    if (ReceiveQueue[i].IsActive) {
      hdr = reinterpret_cast<CommHdr*>(ReceiveQueue[i].Buffer);
      hdr->MagicNumber = hdr->MagicNumber;
      hdr->Code = hdr->Code;
      std::sprintf(txt, "%4d %2d %-5s  ", static_cast<int>(hdr->PacketID),
                   ReceiveQueue[i].IsRead,
                   ConnectionClass::Command_Name(hdr->Code));

      if (DebugSize && (DebugOffset + DebugSize) <= SendQueue[i].BufLen) {
        if (DebugSize == 1) {
          val = *(ReceiveQueue[i].Buffer + DebugOffset);
        } else if (DebugSize == 2) {
          val = *((short*)(ReceiveQueue[i].Buffer + DebugOffset));
        } else if (DebugSize == 4) {
          val = *((int*)(ReceiveQueue[i].Buffer + DebugOffset));
        }
        std::sprintf(txt + std::strlen(txt), "%4d  ", val);

        if (DebugMaxNames && val > 0 && val < DebugMaxNames) {
          std::sprintf(txt + std::strlen(txt), "%-12s  %x", DebugNames[val],
                       ReceiveQueue[i].IsACK);
        } else {
          std::sprintf(txt + std::strlen(txt), "              %x",
                       ReceiveQueue[i].IsACK);
        }
      }
      Mono_Printf(txt);
    } else {
      Mono_Printf("____ __                            _");
    }
  }
#else
  refresh = refresh;
#endif
}
