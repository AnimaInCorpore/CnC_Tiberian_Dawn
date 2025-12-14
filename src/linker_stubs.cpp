#include "legacy/display.h"
#include "legacy/drive.h"
#include "legacy/event.h"
#include "legacy/facing.h"
#include "legacy/flasher.h"
#include "legacy/function.h"
#include "legacy/externs.h"
#include "legacy/gadget.h"
#include "legacy/mission.h"
#include "legacy/options.h"
#include "legacy/msgbox.h"
#include "legacy/msglist.h"
#include "legacy/overlay.h"
#include "legacy/terrain.h"
#include "legacy/trigger.h"
#include "legacy/sidebar.h"
#include "legacy/teamtype.h"
#include "legacy/building.h"
#include "legacy/infantry.h"
#include "legacy/aircraft.h"
#include "legacy/radio.h"
#include "legacy/object.h"
#include "legacy/power.h"
#include "legacy/radar.h"
#include "legacy/scroll.h"
#include "legacy/turret.h"
#include "legacy/unit.h"
#include "legacy/type.h"
#include "legacy/door.h"
#include "legacy/smudge.h"
#include "legacy/bullet.h"
#include "legacy/stage.h"
#include "legacy/cargo.h"
#include "legacy/audio_stub.h"
#include "legacy/ipxmgr.h"
#include "legacy/ipxconn.h"
#include "legacy/ipxgconn.h"
#include "legacy/wwlib32.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstring>

// --- Legacy globals and data tables ----------------------------------------------------------

bool IsTheaterShape = false;
IPXManagerClass Ipx(0, 0, 0, 0, 0, 0);

static constexpr unsigned char kIdentityRemap[256] = {
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  15,
    16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
    32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
    48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
    64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
    80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,
    96,  97,  98,  99,  100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
    112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
    128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
    144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
    160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
    176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
    192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
    208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
    224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
    240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255};

unsigned char const RemapGreen[256] = {};
unsigned char const RemapBlue[256] = {};
unsigned char const RemapOrange[256] = {};
unsigned char const RemapNone[256] = {0};
unsigned char const RemapYellow[256] = {};
unsigned char const RemapRed[256] = {};
unsigned char const RemapBlueGreen[256] = {};

GroundType const Ground[LAND_COUNT] = {};

// --- Lightweight implementations -------------------------------------------------------------

void Fat_Put_Pixel(int x, int y, int color, int size, GraphicViewPortClass& port) {
  for (int dy = 0; dy < size; ++dy) {
    for (int dx = 0; dx < size; ++dx) {
      port.Put_Pixel(x + dx, y + dy, color);
    }
  }
}

int Sound_Effect(VocType, VolType, int, signed short) { return 0; }
void Sound_Effect(VocType, COORDINATE, int) {}

bool WWWritePrivateProfileInt(char const*, char const*, int value, char*) { return value >= 0; }

int Modify_Damage(int damage, WarheadType, ArmorType, int) { return damage; }

void Explosion_Damage(COORDINATE, unsigned, TechnoClass*, WarheadType) {}

bool Queue_Mission(TARGET, MissionType) { return false; }
bool Queue_Mission(TARGET, MissionType, TARGET, TARGET) { return false; }

int Create_Air_Reinforcement(HouseClass*, AircraftType, int, MissionType, TARGET, TARGET) { return 0; }

bool Create_Special_Reinforcement(HouseClass*, TechnoTypeClass const*, TechnoTypeClass const*, TeamMissionType, int) {
  return false;
}

void Speak(VoxType) {}

TechnoTypeClass const* Fetch_Techno_Type(RTTIType, int) { return nullptr; }

int Scan_Place_Object(ObjectClass*, CELL) { return 0; }

void Unselect_All() {}

int CCMessageBox::Process(char const*, char const*, char const*, char const*, bool) { return 0; }

CELL As_Cell(TARGET target) { return static_cast<CELL>(target); }
UnitClass* As_Unit(TARGET) { return nullptr; }
TechnoClass* As_Techno(TARGET) { return nullptr; }
AircraftClass* As_Aircraft(TARGET) { return nullptr; }

// --- Flasher ---------------------------------------------------------------------------------

bool FlasherClass::Process() {
  if (FlashCount) {
    --FlashCount;
    IsBlushing = !IsBlushing;
  }
  return FlashCount != 0;
}

void FlasherClass::Code_Pointers() {}
void FlasherClass::Decode_Pointers() {}

#ifdef CHEAT_KEYS
void FlasherClass::Debug_Dump(MonoClass*) const {}
#endif

// --- Event -----------------------------------------------------------------------------------

EventClass::EventClass(EventType type, TARGET whom) : Type(type), Frame(0), ID(0), IsExecuted(0), MPlayerID(0) {
  Data.Mission.Whom = whom;
  Data.Mission.Mission = MISSION_NONE;
}

void RadarClass::Code_Pointers() {}
void RadarClass::Decode_Pointers() {}

// --- Facing ----------------------------------------------------------------------------------

int FacingClass::Set_Current(DirType dir) {
  CurrentFacing = dir;
  return static_cast<int>(dir);
}
int FacingClass::Set_Desired(DirType dir) {
  DesiredFacing = dir;
  return static_cast<int>(dir);
}
int FacingClass::Rotation_Adjust(int rate) { return rate; }

// --- Missions -------------------------------------------------------------------------------

MissionClass::MissionClass()
    : Mission(MISSION_NONE), SuspendedMission(MISSION_NONE), MissionQueue(MISSION_NONE), Status(0) {}
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
#ifdef CHEAT_KEYS
void MissionClass::Debug_Dump(MonoClass*) const {}
#endif

// --- Options -------------------------------------------------------------------------------

OptionsClass::OptionsClass() = default;
void OptionsClass::One_Time() {}
void OptionsClass::Process() {}
void OptionsClass::Fixup_Palette() const {}
void OptionsClass::Set_Shuffle(int) {}
void OptionsClass::Set_Repeat(int) {}
void OptionsClass::Set_Score_Volume(int) {}
void OptionsClass::Set_Sound_Volume(int, int) {}
void OptionsClass::Set_Brightness(int) {}
int OptionsClass::Get_Brightness() const { return 0; }
void OptionsClass::Set_Color(int) {}

// --- Object / Techno / Unit ------------------------------------------------------------------

void ObjectClass::Code_Pointers() {}
void ObjectClass::Decode_Pointers() {}

void TechnoClass::Code_Pointers() {}
void TechnoClass::Decode_Pointers() {}

void UnitClass::Code_Pointers() {}
void UnitClass::Decode_Pointers() {}

DriveClass::DriveClass()
    : FootClass(),
      Class(nullptr),
      Tiberium(0),
      IsHarvesting(0),
      IsReturning(0),
      IsTurretLockedDown(0),
      IsOnShortTrack(0),
      SpeedAccum(0),
      TrackNumber(0),
      TrackIndex(0) {}

DriveClass::DriveClass(UnitType type, HousesType)
    : DriveClass() {
  (void)type;
}

int DriveClass::Offload_Tiberium_Bail() { return 0; }
void DriveClass::Do_Turn(DirType) {}
void DriveClass::Approach_Target() {}
ObjectTypeClass const& DriveClass::Class_Of() const {
  class StubObjectTypeClass : public ObjectTypeClass {
   public:
    StubObjectTypeClass()
        : ObjectTypeClass(false,
                          false,
                          false,
                          false,
                          false,
                          false,
                          false,
                          false,
                          0,
                          "",
                          ARMOR_NONE,
                          0) {}
    bool Create_And_Place(CELL, HousesType) const override { return false; }
    ObjectClass* Create_One_Of(HouseClass*) const override { return nullptr; }
  };

  static StubObjectTypeClass dummy;
  return dummy;
}
void DriveClass::Overrun_Square(CELL, bool) {}
void DriveClass::Assign_Destination(TARGET) {}
void DriveClass::Per_Cell_Process(bool) {}
bool DriveClass::Ok_To_Move(DirType) const { return true; }
void DriveClass::AI() {}
void DriveClass::Force_Track(int, COORDINATE) {}
int DriveClass::Tiberium_Load() const { return Tiberium; }
void DriveClass::Exit_Map() {}
void DriveClass::Mark_Track(COORDINATE, MarkType) {}
void DriveClass::Code_Pointers() {}
void DriveClass::Decode_Pointers() {}
void DriveClass::Fixup_Path(PathType*) {}

// --- Turret ----------------------------------------------------------------------------------

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

// --- Power -----------------------------------------------------------------------------------

PowerClass::PowerClass()
    : PowX(0),
      PowY(0),
      PowWidth(0),
      PowHeight(0),
      PowLineSpace(0),
      PowLineWidth(0),
      IsToRedraw(0),
      IsActive(0),
      RecordedDrain(0),
      RecordedPower(0),
      DesiredDrainHeight(0),
      DesiredPowerHeight(0),
      DrainHeight(0),
      PowerHeight(0),
      DrainBounce(0),
      PowerBounce(0),
      PowerDir(0),
      DrainDir(0) {}

void PowerClass::One_Time() {}
void PowerClass::Init_Clear() {}
void PowerClass::Draw_It(bool) {}
void PowerClass::AI(KeyNumType&, int, int) {}
void PowerClass::Refresh_Cells(CELL, short const*) {}
void PowerClass::Code_Pointers() {}
void PowerClass::Decode_Pointers() {}

PowerClass::PowerButtonClass PowerClass::PowerButton{};
void const* PowerClass::PowerShape = nullptr;

int PowerClass::Power_Height(int value) { return value; }
int PowerClass::PowerButtonClass::Action(unsigned, KeyNumType&) { return 0; }

// --- Scroll ----------------------------------------------------------------------------------

void ScrollClass::Code_Pointers() {}
void ScrollClass::Decode_Pointers() {}


// --- Smudge ----------------------------------------------------------------------------------

SmudgeClass::SmudgeClass(SmudgeType, COORDINATE, HousesType) : Class(nullptr) {}
void SmudgeClass::Disown(CELL) {}
void* SmudgeClass::operator new(std::size_t size) { return ::operator new(size); }
void SmudgeClass::operator delete(void* ptr) { ::operator delete(ptr); }
void SmudgeClass::Init() {}
void SmudgeClass::Read_INI(char*) {}
void SmudgeClass::Write_INI(char*) {}
bool SmudgeClass::Load(FileClass&) { return true; }
bool SmudgeClass::Save(FileClass&) { return true; }
void SmudgeClass::Code_Pointers() {}
void SmudgeClass::Decode_Pointers() {}
bool SmudgeClass::Mark(MarkType) { return true; }
int SmudgeClass::Validate() const { return 0; }

// --- Bullet ----------------------------------------------------------------------------------

BulletClass::BulletClass() : Class(nullptr), Payback(nullptr), PrimaryFacing() {}
BulletClass::BulletClass(BulletType) : Class(nullptr), Payback(nullptr), PrimaryFacing() {}
void* BulletClass::operator new(std::size_t size) { return ::operator new(size); }
void BulletClass::operator delete(void* ptr) { ::operator delete(ptr); }
bool BulletClass::Unlimbo(COORDINATE, DirType) { return true; }
void BulletClass::Detach(TARGET, bool) {}
void BulletClass::Draw_It(int, int, WindowNumberType) {}
bool BulletClass::Mark(MarkType) { return true; }
void BulletClass::AI() {}
void BulletClass::Code_Pointers() {}
void BulletClass::Decode_Pointers() {}
short const* BulletClass::Occupy_List() const { return nullptr; }
TARGET BulletClass::As_Target() const { return 0; }

// --- Terrain / Overlay / Triggers ------------------------------------------------------------

unsigned char* TerrainClass::Radar_Icon(CELL) { return nullptr; }

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

TriggerClass* TriggerClass::As_Pointer(char const*) { return nullptr; }
bool TriggerClass::Spring(EventType, HousesType, long) { return false; }
bool TriggerClass::Spring(EventType, ObjectClass*) { return false; }
TriggerClass::~TriggerClass() = default;
void* TriggerClass::operator new(std::size_t size) { return ::operator new(size); }
void TriggerClass::operator delete(void* ptr) { ::operator delete(ptr); }

void BuildingClass::Code_Pointers() {}
void BuildingClass::Decode_Pointers() {}

void InfantryClass::Code_Pointers() {}
void InfantryClass::Decode_Pointers() {}

TextLabelClass* MessageListClass::Add_Message(char*, int, TextPrintType, int, unsigned short, unsigned short) {
  return nullptr;
}

TeamTypeClass const* TeamTypeClass::Suggested_New_Team(HouseClass*, long, long, bool) { return nullptr; }
TeamClass* TeamTypeClass::Create_One_Of() const { return nullptr; }
void* TeamTypeClass::operator new(size_t) {
  void* ptr = TeamTypes.Allocate();
  if (ptr) {
    static_cast<TeamTypeClass*>(ptr)->IsActive = true;
  }
  return ptr;
}
void TeamTypeClass::operator delete(void* ptr) {
  if (!ptr) return;
  static_cast<TeamTypeClass*>(ptr)->IsActive = false;
  TeamTypes.Free(static_cast<TeamTypeClass*>(ptr));
}

InfantryTypeClass const* const InfantryTypeClass::Pointers[INFANTRY_COUNT] = {nullptr};
InfantryType InfantryTypeClass::From_Name(char const*) { return INFANTRY_NONE; }
TemplateTypeClass const* const TemplateTypeClass::Pointers[TEMPLATE_COUNT] = {nullptr};
TerrainTypeClass const* const TerrainTypeClass::Pointers[TERRAIN_COUNT] = {nullptr};
SmudgeTypeClass const* const SmudgeTypeClass::Pointers[SMUDGE_COUNT] = {nullptr};
OverlayTypeClass const* const OverlayTypeClass::Pointers[OVERLAY_COUNT] = {nullptr};
UnitTypeClass const* const UnitTypeClass::Pointers[UNIT_COUNT] = {nullptr};
void const* UnitTypeClass::WakeShapes = nullptr;
UnitTypeClass const& UnitTypeClass::As_Reference(UnitType unit) {
  alignas(UnitTypeClass) static unsigned char storage[sizeof(UnitTypeClass)] = {};
  static UnitTypeClass const& dummy = *reinterpret_cast<UnitTypeClass const*>(storage);
  if (unit < UNIT_COUNT && Pointers[unit]) return *Pointers[unit];
  return dummy;
}
UnitType UnitTypeClass::From_Name(char const*) { return UNIT_NONE; }

HouseTypeClass const* const HouseTypeClass::Pointers[HOUSE_COUNT] = {nullptr};
HouseTypeClass const& HouseTypeClass::As_Reference(HousesType house) {
  alignas(HouseTypeClass) static unsigned char storage[sizeof(HouseTypeClass)] = {};
  static HouseTypeClass const& dummy = *reinterpret_cast<HouseTypeClass const*>(storage);
  if (house < HOUSE_COUNT && Pointers[house]) return *Pointers[house];
  return dummy;
}

// --- Cells / Fuses / Animations --------------------------------------------------------------

CellClass::CellClass() = default;
int CellClass::Spot_Index(COORDINATE) { return 0; }
int CellClass::Cell_Color(bool) const { return 0; }
int CellClass::Clear_Icon() const { return 0; }

FuseClass::FuseClass() : Timer(0), Arming(0), HeadTo(0), Proximity(0) {}

// --- Misc ---------------------------------------------------------------------

TerrainClass::TerrainClass(TerrainType, CELL) : Class(nullptr) {}
TerrainClass::~TerrainClass() = default;
void* TerrainClass::operator new(std::size_t size) { return ::operator new(size); }
void TerrainClass::operator delete(void* ptr) { ::operator delete(ptr); }
#ifdef CHEAT_KEYS
void TerrainClass::Debug_Dump(MonoClass*) const {}
#endif
bool TerrainClass::Unlimbo(COORDINATE, DirType) { return true; }
bool TerrainClass::Limbo() { return true; }
MoveType TerrainClass::Can_Enter_Cell(CELL, FacingType) const { return MOVE_OK; }
void TerrainClass::Draw_It(int, int, WindowNumberType) {}
bool TerrainClass::Mark(MarkType) { return true; }
void TerrainClass::Fire_Out() {}
bool TerrainClass::Catch_Fire() { return false; }
ResultType TerrainClass::Take_Damage(int& damage, int, WarheadType, TechnoClass*) {
  damage = 0;
  return RESULT_NONE;
}
TARGET TerrainClass::As_Target() const { return 0; }
COORDINATE TerrainClass::Center_Coord() const { return 0; }
void TerrainClass::AI() {}
void TerrainClass::Read_INI(char*) {}
void TerrainClass::Write_INI(char*) {}
bool TerrainClass::Load(FileClass&) { return true; }
bool TerrainClass::Save(FileClass&) { return true; }
void TerrainClass::Code_Pointers() {}
void TerrainClass::Decode_Pointers() {}
int TerrainClass::Validate() const { return 0; }

#ifdef CHEAT_KEYS
void DriveClass::Debug_Dump(MonoClass*) const {}
void FootClass::Debug_Dump(MonoClass*) const {}
void TarComClass::Debug_Dump(MonoClass*) const {}
void TurretClass::Debug_Dump(MonoClass*) const {}
#endif

namespace {
struct TurretAnchor : TurretClass {
  TurretAnchor() : TurretClass() {}
};

TurretAnchor g_turret_anchor;
}  // namespace
