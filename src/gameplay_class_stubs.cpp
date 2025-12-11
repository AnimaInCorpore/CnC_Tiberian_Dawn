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
void SidebarClass::Code_Pointers(void) {}
void SidebarClass::Decode_Pointers(void) {}

SidebarClass::StripClass::SelectClass::SelectClass()
    : ControlClass(0, 0, 0, 0, 0), Strip(nullptr), Index(0) {}
int SidebarClass::StripClass::SelectClass::Action(unsigned, KeyNumType&) { return 0; }

SidebarClass::StripClass::StripClass() = default;
bool SidebarClass::StripClass::Add(RTTIType, int) { return false; }
bool SidebarClass::StripClass::Abandon_Production(int) { return false; }
bool SidebarClass::StripClass::Scroll(bool) { return false; }
bool SidebarClass::StripClass::AI(KeyNumType&, int, int) { return false; }
void SidebarClass::StripClass::Draw_It(bool) {}
void SidebarClass::StripClass::One_Time(int) {}
void SidebarClass::StripClass::Init_Clear() {}
void SidebarClass::StripClass::Init_IO(int) {}
void SidebarClass::StripClass::Init_Theater(TheaterType) {}
bool SidebarClass::StripClass::Recalc() { return false; }
void SidebarClass::StripClass::Activate() {}
void SidebarClass::StripClass::Deactivate() {}
void SidebarClass::StripClass::Flag_To_Redraw() {}
bool SidebarClass::StripClass::Factory_Link(int, RTTIType, int) { return false; }
void const* SidebarClass::StripClass::Get_Special_Cameo(int) { return nullptr; }

void TabClass::Code_Pointers(void) {}
void TabClass::Decode_Pointers(void) {}

void RadioClass::Code_Pointers(void) {}
void RadioClass::Decode_Pointers(void) {}

FacingClass::FacingClass() : CurrentFacing(DIR_N), DesiredFacing(DIR_N) {}

ThemeClass::ThemeClass() = default;

ThemeType ThemeClass::From_Name(char const*) { return THEME_NONE; }

FootClass::FootClass() : TechnoClass(), Speed(0) {}
FootClass::FootClass(HousesType house) : TechnoClass(house), Speed(0) {}
FootClass::~FootClass() = default;
bool FootClass::Basic_Path() { return false; }
bool FootClass::Unlimbo(COORDINATE, DirType) { return true; }
void FootClass::Sell_Back(int) {}
int FootClass::Offload_Tiberium_Bail(void) { return 0; }
void FootClass::Detach(TARGET, bool) {}
void FootClass::Detach_All(bool) {}
void FootClass::Assign_Mission(MissionType) {}
int FootClass::Mission_Enter(void) { return 0; }
int FootClass::Mission_Move(void) { return 0; }
int FootClass::Mission_Capture(void) { return 0; }
int FootClass::Mission_Attack(void) { return 0; }
int FootClass::Mission_Guard(void) { return 0; }
int FootClass::Mission_Hunt(void) { return 0; }
int FootClass::Mission_Timed_Hunt(void) { return 0; }
int FootClass::Mission_Guard_Area(void) { return 0; }
void FootClass::Per_Cell_Process(bool) {}
void FootClass::Approach_Target(void) {}
void FootClass::Set_Speed(int) {}
void FootClass::Override_Mission(MissionType, TARGET, TARGET) {}
bool FootClass::Restore_Mission(void) { return false; }
void FootClass::Code_Pointers(void) {}
void FootClass::Decode_Pointers(void) {}
int FootClass::Rescue_Mission(TARGET) { return 0; }
void FootClass::Stun(void) {}
bool FootClass::Mark(MarkType) { return false; }
MoveType FootClass::Can_Enter_Cell(CELL, FacingType) const { return MOVE_OK; }
void FootClass::Active_Click_With(ActionType, ObjectClass*) {}
void FootClass::Active_Click_With(ActionType, short) {}
bool FootClass::Start_Driver(COORDINATE&) { return false; }
bool FootClass::Stop_Driver() { return true; }
void FootClass::Assign_Destination(TARGET) {}
bool FootClass::Limbo() { return true; }
bool FootClass::Can_Demolish() const { return false; }
COORDINATE FootClass::Sort_Y() const { return Coord; }
COORDINATE FootClass::Likely_Coord() const { return Coord; }
ResultType FootClass::Take_Damage(int& damage, int, WarheadType, TechnoClass*) {
  damage = 0;
  return RESULT_NONE;
}
RadioMessageType FootClass::Receive_Message(RadioClass*, RadioMessageType message, long&) { return message; }
void FootClass::Death_Announcement(TechnoClass const*) const {}
TARGET FootClass::Greatest_Threat(ThreatType) const { return 0; }

HelpClass::~HelpClass() = default;
HelpClass::HelpClass() : HelpX(0), HelpY(0), HelpWidth(0), Cost(0), X(0), Y(0), DrawX(0), DrawY(0), Width(0), Text(0), IsRight(0) {}
void HelpClass::Init_Clear(void) {}
void HelpClass::Draw_It(bool) {}
void HelpClass::AI(KeyNumType&, int, int) {}
bool HelpClass::Scroll_Map(DirType, int& distance, bool) { distance = 0; return false; }
void HelpClass::Set_Tactical_Position(COORDINATE) {}
void HelpClass::Help_Text(int, int, int, int, bool, int) {}
void HelpClass::Set_Cost(int) {}
short const* HelpClass::Overlap_List(void) const { return nullptr; }
void HelpClass::Code_Pointers(void) {}
void HelpClass::Decode_Pointers(void) {}

MonoClass::MonoClass() = default;
MonoClass::~MonoClass() = default;
void MonoClass::Set_Cursor(int, int) {}
void MonoClass::Text_Print(char const*, int, int, char) {}
void MonoClass::Print(char const*) {}
void MonoClass::Printf(char const*, ...) {}

TarComClass::~TarComClass() = default;
void TarComClass::AI(void) {}
void TarComClass::Code_Pointers(void) {}
void TarComClass::Decode_Pointers(void) {}

void TeamClass::Suspend_Teams(int) {}
bool TeamClass::Remove(FootClass*, int) { return false; }
void TeamClass::Detach(TARGET, bool) {}

int OptionsClass::Normalize_Delay(int delay) const { return delay; }
