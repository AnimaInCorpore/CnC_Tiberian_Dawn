#include "legacy/function.h"
#include "legacy/mixfile.h"
#include "legacy/ccfile.h"
#include "legacy/cdfile.h"
#include "legacy/sidebar.h"
#include "legacy/radio.h"
#include "legacy/radar.h"
#include "legacy/msglist.h"
#include "legacy/goptions.h"
#include "legacy/ipxaddr.h"
#include "legacy/theme.h"
#include "legacy/foot.h"

#include <cstddef>
#include <cstring>

HousesType HouseTypeClass::From_Name(char const*) { return HOUSE_NONE; }

HouseClass* HouseClass::As_Pointer(HousesType) { return nullptr; }

unsigned char const* HouseClass::Remap_Table(bool, bool) const {
  return RemapNone;
}

bool HouseClass::Can_Build(AircraftType, HousesType) const { return false; }

MessageListClass::MessageListClass() = default;
MessageListClass::~MessageListClass() = default;

void GameOptionsClass::Adjust_Variables_For_Resolution() {}
void GameOptionsClass::Process() {}

IPXAddressClass::IPXAddressClass() : NetworkNumber{0}, NodeAddress{0} {}
IPXAddressClass::IPXAddressClass(NetNumType net, NetNodeType node)
    : NetworkNumber{0}, NodeAddress{0} {
  Set_Address(net, node);
}

IPXAddressClass::IPXAddressClass(IPXHeaderType* header)
    : NetworkNumber{0}, NodeAddress{0} {
  Set_Address(header);
}

void IPXAddressClass::Set_Address(NetNumType net, NetNodeType node) {
  std::memcpy(NetworkNumber, net, sizeof(NetworkNumber));
  std::memcpy(NodeAddress, node, sizeof(NodeAddress));
}

void IPXAddressClass::Set_Address(IPXHeaderType* header) {
  if (!header) return;
  Set_Address(header->SourceNetworkNumber, header->SourceNetworkNode);
}

void IPXAddressClass::Get_Address(NetNumType net, NetNodeType node) {
  std::memcpy(net, NetworkNumber, sizeof(NetworkNumber));
  std::memcpy(node, NodeAddress, sizeof(NodeAddress));
}

void IPXAddressClass::Get_Address(IPXHeaderType* header) {
  if (!header) return;
  std::memcpy(header->DestNetworkNumber, NetworkNumber, sizeof(NetworkNumber));
  std::memcpy(header->DestNetworkNode, NodeAddress, sizeof(NodeAddress));
}

bool IPXAddressClass::Is_Broadcast(void) {
  for (unsigned char value : NetworkNumber) {
    if (value != 0xFF) return false;
  }
  for (unsigned char value : NodeAddress) {
    if (value != 0xFF) return false;
  }
  return true;
}

int IPXAddressClass::operator==(IPXAddressClass& addr) {
  return std::memcmp(NetworkNumber, addr.NetworkNumber, sizeof(NetworkNumber)) == 0 &&
         std::memcmp(NodeAddress, addr.NodeAddress, sizeof(NodeAddress)) == 0;
}

int IPXAddressClass::operator!=(IPXAddressClass& addr) { return !(*this == addr); }

int IPXAddressClass::operator>(IPXAddressClass& addr) {
  const int cmp = std::memcmp(NetworkNumber, addr.NetworkNumber, sizeof(NetworkNumber));
  if (cmp != 0) return cmp > 0;
  return std::memcmp(NodeAddress, addr.NodeAddress, sizeof(NodeAddress)) > 0;
}

int IPXAddressClass::operator<(IPXAddressClass& addr) { return addr > *this; }

int IPXAddressClass::operator>=(IPXAddressClass& addr) { return !(*this < addr); }

int IPXAddressClass::operator<=(IPXAddressClass& addr) { return !(*this > addr); }

SidebarClass::SidebarClass() = default;
void SidebarClass::Init_Clear(void) {}
void SidebarClass::Init_Theater(TheaterType) {}
void SidebarClass::Refresh_Cells(CELL, short const*) {}
void SidebarClass::AI(KeyNumType&, int, int) {}
void SidebarClass::Draw_It(bool) {}
void SidebarClass::Init_IO(void) {}
bool SidebarClass::Activate(int) { return false; }
void SidebarClass::One_Time(void) {}

void TabClass::Code_Pointers(void) {}
void TabClass::Decode_Pointers(void) {}

RadarClass::RadarClass() = default;
CELL RadarClass::Click_Cell_Calc(int, int) { return 0; }
void RadarClass::Set_Map_Dimensions(int, int, int, int) {}
void RadarClass::Set_Tactical_Position(COORDINATE) {}
bool RadarClass::Map_Cell(CELL, HouseClass*) { return false; }

void RadioClass::Code_Pointers(void) {}
void RadioClass::Decode_Pointers(void) {}

CreditClass::CreditClass() = default;
void CreditClass::Graphic_Logic(bool) {}
void CreditClass::AI(bool) {}

FacingClass::FacingClass() : CurrentFacing(DIR_N), DesiredFacing(DIR_N) {}

CCFileClass::CCFileClass(char const* filename) : CDFileClass(filename) {}
CCFileClass::CCFileClass() : CDFileClass() {}

int CCFileClass::Open(int rights) { return CDFileClass::Open(rights); }
int CCFileClass::Is_Open(void) const { return RawFileClass::Is_Open(); }
int CCFileClass::Is_Available(int forced) { return CDFileClass::Is_Available(forced); }
long CCFileClass::Read(void* buffer, long size) { return RawFileClass::Read(buffer, size); }
long CCFileClass::Seek(long, int) { return 0; }
long CCFileClass::Size(void) { return RawFileClass::Size(); }
long CCFileClass::Write(void const* buffer, long size) { return RawFileClass::Write(buffer, size); }
void CCFileClass::Close(void) { RawFileClass::Close(); }
void CCFileClass::Error(int, int, char const*) {}

CDFileClass::SearchDriveType* CDFileClass::First = nullptr;
char CDFileClass::RawPath[512] = {};
int CDFileClass::CurrentCDDrive = 0;
int CDFileClass::LastCDDrive = 0;

CDFileClass::CDFileClass(char const* filename) : RawFileClass(filename), IsDisabled(false) {}
CDFileClass::CDFileClass() : RawFileClass(), IsDisabled(false) {}
void CDFileClass::Set_CD_Drive(int drive) {
  CurrentCDDrive = drive;
  LastCDDrive = drive;
}

char const* CDFileClass::Set_Name(char const* filename) { return RawFileClass::Set_Name(filename); }
int CDFileClass::Open(char const* filename, int rights) { return RawFileClass::Open(filename, rights); }
int CDFileClass::Open(int rights) { return RawFileClass::Open(rights); }
int CDFileClass::Set_Search_Drives(char*) { return 0; }
void CDFileClass::Add_Search_Drive(char*) {}
void CDFileClass::Clear_Search_Drives(void) {
  First = nullptr;
  RawPath[0] = '\0';
}
void CDFileClass::Refresh_Search_Drives(void) {}

RawFileClass::RawFileClass(char const* filename) : Rights(READ), Handle(-1), Filename(nullptr), Allocated(false) {
  Set_Name(filename);
}

RawFileClass::RawFileClass(RawFileClass const& file)
    : Rights(file.Rights), Handle(-1), Filename(file.Filename), Allocated(false) {}

RawFileClass& RawFileClass::operator=(RawFileClass const& file) {
  if (this != &file) {
    Rights = file.Rights;
    Handle = -1;
    Filename = file.Filename;
    Allocated = false;
  }
  return *this;
}

RawFileClass::~RawFileClass() { Close(); }

char const* RawFileClass::Set_Name(char const* filename) {
  Filename = filename;
  Allocated = false;
  return Filename;
}

int RawFileClass::Create(void) { return 0; }

int RawFileClass::Delete(void) { return 0; }

int RawFileClass::Is_Available(int) { return 0; }

int RawFileClass::Open(char const* filename, int rights) { return Set_Name(filename) ? Open(rights) : 0; }

int RawFileClass::Open(int rights) {
  Rights = rights;
  Handle = 0;
  return 1;
}

long RawFileClass::Read(void* buffer, long size) {
  if (buffer && size > 0) {
    std::memset(buffer, 0, static_cast<std::size_t>(size));
  }
  return size;
}

long RawFileClass::Seek(long, int) { return 0; }

long RawFileClass::Size(void) { return 0; }

long RawFileClass::Write(void const*, long size) { return size; }

void RawFileClass::Close(void) { Handle = -1; }

void RawFileClass::Error(int, int, char const*) {}

MixFileClass* MixFileClass::First = nullptr;

MixFileClass::MixFileClass(char const* filename)
    : Filename(filename), Count(0), DataSize(0), Buffer(nullptr), Data(nullptr) {}

MixFileClass::~MixFileClass() = default;

bool MixFileClass::Free(char const*) { return false; }

void MixFileClass::Free_All(void) {}

void MixFileClass::Free(void) {}

bool MixFileClass::Cache(void) { return false; }

bool MixFileClass::Cache(char const*) { return false; }

bool MixFileClass::Offset(char const*, void**, MixFileClass**, long*, long*) { return false; }

void const* MixFileClass::Retrieve(char const*) { return nullptr; }

ThemeClass::ThemeClass() = default;

ThemeType ThemeClass::From_Name(char const*) { return THEME_NONE; }

FootClass::FootClass() : TechnoClass(), Speed(0) {}
FootClass::FootClass(HousesType house) : TechnoClass(house), Speed(0) {}
FootClass::~FootClass() = default;
bool FootClass::Unlimbo(COORDINATE, DirType) { return true; }
