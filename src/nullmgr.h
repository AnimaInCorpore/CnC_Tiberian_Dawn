#pragma once

#include "wincomm.h"

/*
** Portable build note:
** The original NULLMGR module depends on the legacy Win32/Greenleaf serial
** stack. For the CMake + SDL 1.2 build, keep a minimal API surface so other
** modules can compile and link while the real backend is ported.
*/

class NullModemClass {
public:
    char* BuildBuf;
    int MaxLen;

    NullModemClass(int numsend, int numreceive, int maxlen, unsigned short magicnum);
    ~NullModemClass();

    int Init(int port,
             int irq,
             char* dev_name,
             int baud,
             char parity,
             int wordlength,
             int stopbits,
             int flowcontrol);
    int Delete_Connection(void);
    void Shutdown(void);

    int Send_Message(void* buf, int buflen, int ack_req = 1);
    int Get_Message(void* buf, int* buflen);
    int Service(void);

    int Num_Send(void);
    int Num_Receive(void);
    unsigned long Response_Time(void);
    void Reset_Response_Time(void);
    void* Oldest_Send(void);

    DetectPortType Detect_Port(SerialSettingsType* settings);
    int Detect_Modem(SerialSettingsType* settings, bool reconnect = false);
    DialStatusType Dial_Modem(char* string, DialMethodType method, bool reconnect = false);
    DialStatusType Answer_Modem(bool reconnect = false);
    bool Hangup_Modem(void);
    int Get_Modem_Status(void);
    int Change_IRQ_Priority(int irq);

private:
    int NumSend;
    int NumReceive;
    unsigned short MagicNum;
};

extern NullModemClass NullModem;

