#include "legacy/goptions.h"
#include "legacy/defines.h"
#include "legacy/txtlabel.h"
#include "legacy/msglist.h"
#include "legacy/help.h"
#include "legacy/foot.h"

// Minimal game options hooks
void GameOptionsClass::Adjust_Variables_For_Resolution() {}
void GameOptionsClass::Process() {}

// Message list minimal: ctor/dtor implemented in tiny_linker_shims.cpp

// FootClass minimal implementations used by various vtables
FootClass::FootClass() : TechnoClass(), Speed(0) {}
FootClass::FootClass(HousesType house) : TechnoClass(house), Speed(0) {}
	FootClass::~FootClass() = default; // Destructor implementation

bool FootClass::Basic_Path() { return false; }
void FootClass::Detach_All(bool) {}
bool FootClass::Stop_Driver() { return true; }
ResultType FootClass::Take_Damage(int& damage, int, WarheadType, TechnoClass*) { damage = 0; return RESULT_NONE; }
int FootClass::Mission_Hunt() { return 0; }
int FootClass::Mission_Move() { return 0; }
int FootClass::Mission_Enter() { return 0; }
int FootClass::Mission_Guard() { return 0; }
int FootClass::Mission_Attack() { return 0; }
int FootClass::Mission_Capture() { return 0; }
int FootClass::Mission_Timed_Hunt() { return 0; }
int FootClass::Mission_Guard_Area() { return 0; }
// Mission_Stop/Unload/Repair are not declared in FootClass (removed)

void FootClass::Assign_Mission(MissionType) {}
int FootClass::Rescue_Mission(TARGET) { return 0; }
void FootClass::Approach_Target() {}
void FootClass::Per_Cell_Process(bool) {}
	// void FootClass::Set_Speed(int) {} // previously implemented
void FootClass::Override_Mission(MissionType, TARGET, TARGET) {}
bool FootClass::Restore_Mission() { return false; }
void FootClass::Stun() {}
bool FootClass::Mark(MarkType) { return false; }
MoveType FootClass::Can_Enter_Cell(CELL, FacingType) const { return MOVE_OK; }
COORDINATE FootClass::Sort_Y() const { return Coord; }
COORDINATE FootClass::Likely_Coord() const { return Coord; }
TARGET FootClass::Greatest_Threat(ThreatType) const { return 0; }
void FootClass::Active_Click_With(ActionType, ObjectClass*) {}
void FootClass::Active_Click_With(ActionType, short) {}
bool FootClass::Start_Driver(COORDINATE&) { return false; }
bool FootClass::Limbo() { return true; }
bool FootClass::Can_Demolish() const { return false; }

	// Additional FootClass hooks required by vtables
	void FootClass::Code_Pointers() {}
	void FootClass::Decode_Pointers() {}
	RadioMessageType FootClass::Receive_Message(RadioClass*, RadioMessageType message, long&) { return message; }
	void FootClass::Assign_Destination(TARGET) {}
	int FootClass::Offload_Tiberium_Bail() { return 0; }
	void FootClass::Detach(TARGET, bool) {}
	bool FootClass::Unlimbo(COORDINATE, DirType) { return true; }
	void FootClass::Sell_Back(int) {}
	void FootClass::Death_Announcement(TechnoClass const*) const {}

	// HelpClass minimal destructor (was provided in gameplay_class_stubs previously)
	HelpClass::~HelpClass() = default;
	void HelpClass::Code_Pointers() {}
	void HelpClass::Decode_Pointers() {}

	int OptionsClass::Normalize_Delay(int delay) const { return delay; }

	bool TeamClass::Remove(FootClass*, int) { return false; }

void FootClass::Set_Speed(int) {}
