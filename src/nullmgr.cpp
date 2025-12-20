#include "legacy/nullmgr.h"

#include "legacy/function.h"
#include "legacy/tcpip.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>

void (*NullModemClass::OrigAbortModemFunc)(int) = nullptr;
KeyNumType NullModemClass::Input = 0;
GadgetClass *NullModemClass::Commands = nullptr;

namespace {

constexpr char kDefaultHost[] = "127.0.0.1";

void Ensure_Protocol() {
  Winsock.Set_Protocol_UDP(TRUE);
  if (!Winsock.Get_Connected()) {
    Winsock.Init();
  }
}

}  // namespace

NullModemClass::NullModemClass(int numsend, int numreceive, int maxlen,
                               unsigned short magicnum)
    : ConnManClass(),
      BuildBuf(nullptr),
      MaxLen(maxlen),
      EchoBuf(nullptr),
      EchoSize(500),
      EchoCount(0),
      OldIRQPri(-1),
      ModemVerboseOn(false),
      ModemEchoOn(false),
      ModemWaitCarrier(50000),
      ModemCarrierDetect(600),
      ModemCarrierLoss(1400),
      ModemHangupDelay(20000),
      ModemGuardTime(1000),
      ModemEscapeCode('+'),
      Connection(nullptr),
      NumConnections(0),
      NumSend(numsend),
      NumReceive(numreceive),
      MagicNum(magicnum),
      RXBuf(nullptr),
      RXSize(0),
      RXCount(0),
      RetryDelta(60),
      MaxRetries(static_cast<unsigned long>(-1)),
      Timeout(1200),
      SendOverflows(0),
      ReceiveOverflows(0),
      CRCErrors(0) {}

NullModemClass::~NullModemClass() { Delete_Connection(); }

int NullModemClass::Init(int, int, char *, int, char, int, int, int) {
  Delete_Connection();

  Connection = new NullModemConnClass(NumSend, NumReceive, MaxLen, MagicNum);
  Connection->Set_Retry_Delta(RetryDelta);
  Connection->Set_Max_Retries(MaxRetries);
  Connection->Set_TimeOut(Timeout);
  Connection->Init(nullptr);

  RXSize = Connection->Actual_Max_Packet() * NumReceive;
  RXBuf = new char[RXSize];
  RXCount = 0;

  BuildBuf = new char[MaxLen];
  EchoBuf = new char[EchoSize];
  EchoCount = 0;

  NumConnections = 1;
  Ensure_Protocol();
  return true;
}

int NullModemClass::Delete_Connection(void) {
  delete Connection;
  Connection = nullptr;

  delete[] RXBuf;
  RXBuf = nullptr;
  RXSize = 0;
  RXCount = 0;

  delete[] BuildBuf;
  BuildBuf = nullptr;

  delete[] EchoBuf;
  EchoBuf = nullptr;
  EchoCount = 0;

  NumConnections = 0;
  return true;
}

int NullModemClass::Num_Connections(void) { return NumConnections; }

int NullModemClass::Init_Send_Queue(void) {
  if (Connection) {
    Connection->Queue->Init_Send_Queue();
  }
  return true;
}

void NullModemClass::Shutdown(void) {
  Winsock.Close();
  Delete_Connection();
}

void NullModemClass::Set_Timing(unsigned long retrydelta, unsigned long maxretries,
                                unsigned long timeout) {
  RetryDelta = retrydelta;
  MaxRetries = maxretries;
  Timeout = timeout;

  if (Connection) {
    Connection->Set_Retry_Delta(RetryDelta);
    Connection->Set_Max_Retries(MaxRetries);
    Connection->Set_TimeOut(Timeout);
  }
}

int NullModemClass::Send_Message(void *buf, int buflen, int ack_req) {
  if (NumConnections == 0 || !Connection) {
    return false;
  }
  const int rc = Connection->Send_Packet(buf, buflen, ack_req);
  if (!rc) {
    SendOverflows++;
  }
  return rc;
}

int NullModemClass::Get_Message(void *buf, int *buflen) {
  if (NumConnections == 0 || !Connection) {
    return false;
  }
  return Connection->Get_Packet(buf, buflen);
}

int NullModemClass::Service(void) {
  if (NumConnections == 0 || !Connection) {
    return false;
  }

  Ensure_Protocol();

  while (RXSize - RXCount > 0) {
    const int got = Winsock.Read(RXBuf + RXCount, RXSize - RXCount);
    if (got <= 0) {
      break;
    }
    RXCount += got;
  }

  if (RXCount < (PACKET_SERIAL_OVERHEAD_SIZE + 1)) {
    return Connection->Service();
  }

  int pos = -1;
  for (int i = 0; i <= RXCount - static_cast<int>(sizeof(short)); i++) {
    if (*reinterpret_cast<unsigned short *>(RXBuf + i) == PACKET_SERIAL_START) {
      pos = i;
      break;
    }
  }

  if (pos == -1) {
    std::memmove(RXBuf, RXBuf + RXCount - (sizeof(short) - 1),
                 sizeof(short) - 1);
    RXCount = sizeof(short) - 1;
    return Connection->Service();
  }

  if ((RXCount - pos) < static_cast<int>(sizeof(SerialHeaderType))) {
    std::memmove(RXBuf, RXBuf + pos, RXCount - pos);
    RXCount -= pos;
    return Connection->Service();
  }

  auto *header = reinterpret_cast<SerialHeaderType *>(RXBuf + pos);
  if (header->MagicNumber2 != PACKET_SERIAL_VERIFY) {
    pos += sizeof(short);
    std::memmove(RXBuf, RXBuf + pos, RXCount - pos);
    RXCount -= pos;
    return Connection->Service();
  }

  const unsigned short length = header->Length;
  if (length > MaxLen) {
    pos += sizeof(short);
    std::memmove(RXBuf, RXBuf + pos, RXCount - pos);
    RXCount -= pos;
    return Connection->Service();
  }

  if ((pos + length + PACKET_SERIAL_OVERHEAD_SIZE) > RXCount) {
    if (pos) {
      std::memmove(RXBuf, RXBuf + pos, RXCount - pos);
      RXCount -= pos;
    }
    return Connection->Service();
  }

  auto *crc = reinterpret_cast<SerialCRCType *>(RXBuf + pos +
                                                sizeof(SerialHeaderType) + length);
  if (NullModemConnClass::Compute_CRC(
          RXBuf + pos + sizeof(SerialHeaderType), length) != crc->SerialCRC) {
    CRCErrors++;
    pos += sizeof(short);
    std::memmove(RXBuf, RXBuf + pos, RXCount - pos);
    RXCount -= pos;
    return Connection->Service();
  }

  if (!Connection->Receive_Packet(RXBuf + pos + sizeof(SerialHeaderType), length)) {
    ReceiveOverflows++;
  }

  pos += (PACKET_SERIAL_OVERHEAD_SIZE + length);
  std::memmove(RXBuf, RXBuf + pos, RXCount - pos);
  RXCount -= pos;

  return Connection->Service();
}

int NullModemClass::Num_Send(void) {
  if (Connection) {
    return Connection->Queue->Num_Send();
  }
  return 0;
}

int NullModemClass::Num_Receive(void) {
  if (Connection) {
    return Connection->Queue->Num_Receive();
  }
  return 0;
}

unsigned long NullModemClass::Response_Time(void) {
  if (Connection) {
    return Connection->Queue->Avg_Response_Time();
  }
  return 0;
}

void NullModemClass::Reset_Response_Time(void) {
  if (Connection) {
    Connection->Queue->Reset_Response_Time();
  }
}

void *NullModemClass::Oldest_Send(void) {
  if (!Connection) {
    return nullptr;
  }

  for (int i = 0; i < Connection->Queue->Num_Send(); i++) {
    SendQueueType *send_entry = Connection->Queue->Get_Send(i);
    if (!send_entry) {
      continue;
    }
    auto *packet = reinterpret_cast<CommHeaderType *>(send_entry->Buffer);
    if (packet->Code == ConnectionClass::PACKET_DATA_ACK && send_entry->IsACK == 0) {
      return send_entry->Buffer;
    }
  }

  return nullptr;
}

void NullModemClass::Configure_Debug(int, int offset, int size, char **names,
                                     int maxnames) {
  if (Connection) {
    Connection->Queue->Configure_Debug(offset, size, names, maxnames);
  }
}

void NullModemClass::Mono_Debug_Print(int, int refresh) {
  if (Connection) {
    Connection->Queue->Mono_Debug_Print(refresh);
  }
}

DetectPortType NullModemClass::Detect_Port(SerialSettingsType *) {
  Ensure_Protocol();
  return Winsock.Get_Connected() ? PORT_VALID : PORT_INVALID;
}

int NullModemClass::Detect_Modem(SerialSettingsType *, bool) {
  Ensure_Protocol();
  return Winsock.Get_Connected() ? 1 : 0;
}

DialStatusType NullModemClass::Dial_Modem(char *string, DialMethodType, bool) {
  Ensure_Protocol();
  if (string && string[0] != '\0') {
    Winsock.Set_Host_Address(string);
  } else {
    Winsock.Set_Host_Address(const_cast<char *>(kDefaultHost));
  }
  Winsock.Start_Client();
  return Winsock.Get_Connected() ? DIAL_CONNECTED : DIAL_ERROR;
}

DialStatusType NullModemClass::Answer_Modem(bool) {
  Ensure_Protocol();
  Winsock.Start_Server();
  return Winsock.Get_Connected() ? DIAL_CONNECTED : DIAL_ERROR;
}

bool NullModemClass::Hangup_Modem(void) {
  Winsock.Close();
  return true;
}

void NullModemClass::Setup_Modem_Echo(void (*func)(char c)) {
  ModemEchoOn = true;
  if (!func || !EchoBuf || EchoCount <= 0) {
    return;
  }
  for (int i = 0; i < EchoCount; i++) {
    func(EchoBuf[i]);
  }
}

void NullModemClass::Remove_Modem_Echo(void) { ModemEchoOn = false; }

void NullModemClass::Print_EchoBuf(void) {
  if (!EchoBuf || EchoCount <= 0) {
    return;
  }
  EchoBuf[std::min(EchoCount, EchoSize - 1)] = '\0';
  CCDebugString(EchoBuf);
}

void NullModemClass::Reset_EchoBuf(void) { EchoCount = 0; }

int NullModemClass::Abort_Modem(void) { return 0; }

void NullModemClass::Setup_Abort_Modem(void) {}

void NullModemClass::Remove_Abort_Modem(void) {}

int NullModemClass::Change_IRQ_Priority(int irq) {
  const int previous = OldIRQPri;
  OldIRQPri = irq;
  return previous;
}

int NullModemClass::Get_Modem_Status(void) { return 0; }

int NullModemClass::Send_Modem_Command(char *, char, char *buffer, int buflen,
                                       int, int) {
  if (buffer && buflen > 0) {
    buffer[0] = '\0';
  }
  return MODEM_CMD_OK;
}

int NullModemClass::Verify_And_Convert_To_Int(char *buffer) {
  if (!buffer) {
    return 0;
  }
  return std::atoi(buffer);
}
