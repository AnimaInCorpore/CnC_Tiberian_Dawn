/*
**	Command & Conquer(tm)
**
**	Portable build note:
**	The original `NULLMGR.CPP` depends on the legacy Win32/Greenleaf serial
**	stack and modem control code. Keep a stub translation unit here so the
**	CMake + SDL 1.2 build remains stable while those dependencies are ported.
*/

#include "nullmgr.h"

static char NullModemBuildBuf[1024];

NullModemClass NullModem(0, 0, static_cast<int>(sizeof(NullModemBuildBuf)), 0);

NullModemClass::NullModemClass(int numsend, int numreceive, int maxlen, unsigned short magicnum)
    : BuildBuf(NullModemBuildBuf), MaxLen(maxlen), NumSend(numsend), NumReceive(numreceive), MagicNum(magicnum)
{
    (void)MagicNum;
}

NullModemClass::~NullModemClass() {}

int NullModemClass::Init(int /*port*/,
                         int /*irq*/,
                         char* /*dev_name*/,
                         int /*baud*/,
                         char /*parity*/,
                         int /*wordlength*/,
                         int /*stopbits*/,
                         int /*flowcontrol*/)
{
    return 0;
}

int NullModemClass::Delete_Connection(void)
{
    return 0;
}

void NullModemClass::Shutdown(void) {}

int NullModemClass::Send_Message(void* /*buf*/, int /*buflen*/, int /*ack_req*/)
{
    return 0;
}

int NullModemClass::Get_Message(void* /*buf*/, int* /*buflen*/)
{
    return 0;
}

int NullModemClass::Service(void)
{
    return 0;
}

int NullModemClass::Num_Send(void)
{
    return 0;
}

int NullModemClass::Num_Receive(void)
{
    return 0;
}

unsigned long NullModemClass::Response_Time(void)
{
    return 0;
}

void NullModemClass::Reset_Response_Time(void) {}

void* NullModemClass::Oldest_Send(void)
{
    return 0;
}

DetectPortType NullModemClass::Detect_Port(SerialSettingsType* /*settings*/)
{
    return PORT_INVALID;
}

int NullModemClass::Detect_Modem(SerialSettingsType* /*settings*/, bool /*reconnect*/)
{
    return 0;
}

DialStatusType NullModemClass::Dial_Modem(char* /*string*/, DialMethodType /*method*/, bool /*reconnect*/)
{
    return DIAL_ERROR;
}

DialStatusType NullModemClass::Answer_Modem(bool /*reconnect*/)
{
    return DIAL_ERROR;
}

bool NullModemClass::Hangup_Modem(void)
{
    return false;
}

int NullModemClass::Get_Modem_Status(void)
{
    return 0;
}

int NullModemClass::Change_IRQ_Priority(int /*irq*/)
{
    return 0;
}

