#include "legacy/function.h"
#include "legacy/sidebar.h"
#include "legacy/mission.h"
#include "legacy/tarcom.h"

// Minimal shims to satisfy linker for gameplay-related virtual methods

// Sidebar minimal implementations (non-duplicating)
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

// TarCom minimal implementations
TarComClass::~TarComClass() = default;
void TarComClass::AI(void) {}
void TarComClass::Code_Pointers(void) {}
void TarComClass::Decode_Pointers(void) {}
void TarComClass::Debug_Dump(MonoClass*) const {}

// Turret minimal implementations
TurretClass::TurretClass() : DriveClass(), Reload(0), SecondaryFacing() {}
TurretClass::TurretClass(UnitType type, HousesType house) : DriveClass(type, house), Reload(0), SecondaryFacing() {}
TurretClass::~TurretClass() = default;
bool TurretClass::Unlimbo(COORDINATE, DirType) { return true; }
FireErrorType TurretClass::Can_Fire(TARGET, int) const { return FIRE_OK; }
DirType TurretClass::Fire_Direction() const { return DIR_N; }
COORDINATE TurretClass::Fire_Coord(int) const { return 0; }
bool TurretClass::Ok_To_Move(DirType) { return true; }
BulletClass* TurretClass::Fire_At(TARGET, int) { return nullptr; }
void TurretClass::AI() {}
void TurretClass::Code_Pointers() {}
void TurretClass::Decode_Pointers() {}
void TurretClass::Debug_Dump(MonoClass*) const {}

// CCMessageBox minimal
int CCMessageBox::Process(char const*, char const*, char const*, char const*, bool) { return 0; }

// Flasher minimal
bool FlasherClass::Process() { if (FlashCount) { --FlashCount; IsBlushing = !IsBlushing; } return FlashCount != 0; }

// Mission minimal
MissionClass::MissionClass() : Mission(MISSION_NONE), SuspendedMission(MISSION_NONE), MissionQueue(MISSION_NONE), Status(0) {}
MissionClass::~MissionClass() = default;
MissionType MissionClass::Get_Mission() const { return Mission; }
void MissionClass::Assign_Mission(MissionType mission) { Mission = mission; }
bool MissionClass::Commence() { return true; }
void MissionClass::AI() {}
int MissionClass::Mission_Sleep() { return 0; }
int MissionClass::Mission_Ambush() { return 0; }
int MissionClass::Mission_Attack() { return 0; }
int MissionClass::Mission_Capture() { return 0; }
int MissionClass::Mission_Guard() { return 0; }
int MissionClass::Mission_Guard_Area() { return 0; }
int MissionClass::Mission_Harvest() { return 0; }
int MissionClass::Mission_Hunt() { return 0; }
int MissionClass::Mission_Timed_Hunt() { return 0; }
int MissionClass::Mission_Move() { return 0; }
int MissionClass::Mission_Retreat() { return 0; }
int MissionClass::Mission_Return() { return 0; }
int MissionClass::Mission_Stop() { return 0; }
int MissionClass::Mission_Unload() { return 0; }
int MissionClass::Mission_Enter() { return 0; }
int MissionClass::Mission_Construction() { return 0; }
int MissionClass::Mission_Deconstruction() { return 0; }
int MissionClass::Mission_Repair() { return 0; }
int MissionClass::Mission_Missile() { return 0; }
void MissionClass::Set_Mission(MissionType mission) { Mission = mission; }
char const* MissionClass::Mission_Name(MissionType) { return "Mission"; }
MissionType MissionClass::Mission_From_Name(char const*) { return MISSION_NONE; }
void MissionClass::Override_Mission(MissionType mission, TARGET, TARGET) { Mission = mission; }
bool MissionClass::Restore_Mission() { return true; }
void MissionClass::Code_Pointers() {}
void MissionClass::Decode_Pointers() {}
void MissionClass::Debug_Dump(MonoClass*) const {}

// Overlay minimal
OverlayClass::OverlayClass() : Class(nullptr) {}
OverlayClass::OverlayClass(OverlayType, CELL, HousesType) : Class(nullptr) {}
void* OverlayClass::operator new(std::size_t size) { return ::operator new(size); }
void OverlayClass::operator delete(void* ptr) { ::operator delete(ptr); }
void OverlayClass::Init() {}
void OverlayClass::Read_INI(char*) {}
void OverlayClass::Write_INI(char*) {}
bool OverlayClass::Load(FileClass&) { return false; }
bool OverlayClass::Save(FileClass&) { return false; }
void OverlayClass::Code_Pointers() {}
void OverlayClass::Decode_Pointers() {}
bool OverlayClass::Mark(MarkType) { return true; }
int OverlayClass::Validate() const { return 0; }

// Terrain/Trigger minimal
unsigned char* TerrainClass::Radar_Icon(CELL) { return nullptr; }
TriggerClass* TriggerClass::As_Pointer(char const*) { return nullptr; }
bool TriggerClass::Spring(EventType, HousesType, long) { return false; }
bool TriggerClass::Spring(EventType, ObjectClass*) { return false; }
TriggerClass::~TriggerClass() = default;
void* TriggerClass::operator new(std::size_t size) { return ::operator new(size); }
void TriggerClass::operator delete(void* ptr) { ::operator delete(ptr); }

