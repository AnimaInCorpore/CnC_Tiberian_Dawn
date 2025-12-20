#include "legacy/function.h"
#include "legacy/terrain.h"
#include "legacy/ipxmgr.h"
#include "legacy/target.h"
#include "legacy/msglist.h"
#include "legacy/power.h"
#include "legacy/radar.h"
#include "legacy/radio.h"
#include "legacy/bullet.h"
#include "legacy/object.h"
#include "legacy/scroll.h"
#include "legacy/techno.h"
#include "legacy/tab.h"
#include "legacy/unit.h"
#include "legacy/building.h"
#include "legacy/infantry.h"
#include "legacy/options.h"
#include "legacy/sidebar.h"
#include "legacy/teamtype.h"

#include <cstring>

bool IsTheaterShape = false;
IPXManagerClass Ipx(0, 0, 0, 0, 0, 0);

unsigned char const RemapGreen[256] = {};
unsigned char const RemapBlue[256] = {};
unsigned char const RemapOrange[256] = {};
unsigned char const RemapNone[256] = {0};
unsigned char const RemapYellow[256] = {};
unsigned char const RemapRed[256] = {};
unsigned char const RemapBlueGreen[256] = {};

GroundType const Ground[LAND_COUNT] = {};

void Fat_Put_Pixel(int x, int y, int color, int size, GraphicViewPortClass &port) {
  for (int dy = 0; dy < size; ++dy)
    for (int dx = 0; dx < size; ++dx)
      port.Put_Pixel(x + dx, y + dy, color);
}

CELL As_Cell(TARGET target) { return static_cast<CELL>(target); }
UnitClass *As_Unit(TARGET) { return nullptr; }
TechnoClass *As_Techno(TARGET) { return nullptr; }
AircraftClass *As_Aircraft(unsigned short) { return nullptr; }

// Stubbed mission helpers are defined in linker_small.cpp when necessary; avoid
// duplicating them here.
// bool Queue_Mission(TARGET, MissionType) { return false; }
// bool Queue_Mission(TARGET, MissionType, TARGET, TARGET) { return false; }

// int Create_Air_Reinforcement(HouseClass *, AircraftType, int, MissionType, TARGET, TARGET) { return 0; }

// Platform/profile helpers are provided by port_stubs.cpp; comment out the
// duplicates here to prevent linker conflicts.
// bool WWWritePrivateProfileInt(char const *, char const *, int value, char *) { return value >= 0; }
// int WWGetPrivateProfileInt(char const *, char const *, int def, char *) { return def; }

int Scan_Place_Object(ObjectClass *, CELL) { return 0; }
// Use port_stubs.cpp implementations for these helpers to avoid duplicate
// linkage.
// int Distance_Coord(COORDINATE, COORDINATE) { return 0; }
// void const *Hires_Retrieve(char const *) { return nullptr; }
// void Validate_Error(char const *) {}
// int Version_Number(void) { return 0; }
// void *Load_Alloc_Data(FileClass &) { return nullptr; }

void Unselect_All() {}

TechnoTypeClass const* Fetch_Techno_Type(RTTIType, int) { return nullptr; }

bool Create_Special_Reinforcement(HouseClass*, TechnoTypeClass const*, TechnoTypeClass const*, TeamMissionType, int) { return false; }

// Minimal DriveClass helpers (only methods referenced by other classes)
void DriveClass::Fixup_Path(PathType*) {}
void DriveClass::Mark_Track(COORDINATE, MarkType) {}
void DriveClass::Overrun_Square(CELL, bool) {}
void DriveClass::Assign_Destination(TARGET) {}
void DriveClass::Per_Cell_Process(bool) {}
int DriveClass::Offload_Tiberium_Bail() { return 0; }
void DriveClass::Exit_Map() {}

void DriveClass::Code_Pointers() {}
void DriveClass::Decode_Pointers() {}
void DriveClass::AI() {}
void DriveClass::Debug_Dump(MonoClass*) const {}

ThemeClass::ThemeClass() = default;
void ThemeClass::Queue_Song(ThemeType) {}

// Parameterized DriveClass constructor (referenced by TurretClass etc.)
DriveClass::DriveClass(UnitType type, HousesType house) : DriveClass() { (void)type; (void)house; }

// MonoClass and mono helpers are provided by src/monoc.cpp in the port build.

// Smudge minimal definitions
SmudgeClass::SmudgeClass(SmudgeType type, COORDINATE pos, HousesType house) : SmudgeClass() { (void)type; (void)pos; (void)house; }
void* SmudgeClass::operator new(std::size_t size) { return ::operator new(size); }
void SmudgeClass::operator delete(void* ptr) { ::operator delete(ptr); }
void SmudgeClass::Disown(CELL) {}
void SmudgeClass::Init() {}
void SmudgeClass::Read_INI(char*) {}
void SmudgeClass::Write_INI(char*) {}
bool SmudgeClass::Load(FileClass&) { return true; }
bool SmudgeClass::Save(FileClass&) { return true; }
void SmudgeClass::Code_Pointers() {}
void SmudgeClass::Decode_Pointers() {}
bool SmudgeClass::Mark(MarkType) { return true; }
int SmudgeClass::Validate() const { return 0; }

// Message list minimal
TextLabelClass* MessageListClass::Add_Message(char* txt, int color, TextPrintType t, int a, unsigned short b, unsigned short c) { (void)txt; (void)color; (void)t; (void)a; (void)b; (void)c; return nullptr; }
MessageListClass::MessageListClass() {}
MessageListClass::~MessageListClass() {}

// Team minimal
void TeamClass::Suspend_Teams(int priority) { (void)priority; }
void TeamClass::Detach(TARGET target, bool all) { (void)target; (void)all; }

// Various Code/Decode pointer no-ops to satisfy vtable references from non-ported IO sources.
void PowerClass::Code_Pointers() {}
void PowerClass::Decode_Pointers() {}
void RadarClass::Code_Pointers() {}
void RadarClass::Decode_Pointers() {}
void RadioClass::Code_Pointers() {}
void RadioClass::Decode_Pointers() {}
void BulletClass::Code_Pointers() {}
void BulletClass::Decode_Pointers() {}
void ObjectClass::Code_Pointers() {}
void ObjectClass::Decode_Pointers() {}
void ScrollClass::Code_Pointers() {}
void ScrollClass::Decode_Pointers() {}
void TechnoClass::Code_Pointers() {}
void TechnoClass::Decode_Pointers() {}
void TabClass::Code_Pointers() {}
void TabClass::Decode_Pointers() {}
void UnitClass::Code_Pointers() {}
void UnitClass::Decode_Pointers() {}
void BuildingClass::Code_Pointers() {}
void BuildingClass::Decode_Pointers() {}
void InfantryClass::Code_Pointers() {}
void InfantryClass::Decode_Pointers() {}

// Options minimal
OptionsClass::OptionsClass() = default;
void OptionsClass::One_Time() {}

// Sidebar strip constructor (small stub)
SidebarClass::StripClass::StripClass() = default;

// TeamType helper stub
TeamTypeClass const* TeamTypeClass::Suggested_New_Team(HouseClass*, long, long, bool) { return nullptr; }
// Unit type minimal symbols
void const* UnitTypeClass::WakeShapes = nullptr;
UnitTypeClass const& UnitTypeClass::As_Reference(UnitType) {
  static UnitTypeClass* dummy = reinterpret_cast<UnitTypeClass*>(operator new(sizeof(UnitTypeClass)));
  return *dummy;
}
UnitType UnitTypeClass::From_Name(char const*) { return UNIT_NONE; }

// Foot debug dump stub
void FootClass::Debug_Dump(MonoClass*) const {}

// TeamType minimal
TeamClass* TeamTypeClass::Create_One_Of() const { return nullptr; }
