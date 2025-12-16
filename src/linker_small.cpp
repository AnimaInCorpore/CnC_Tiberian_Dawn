#include "type.h"
#include "cell.h"
#include "drive.h"
#include "flasher.h"
#include "options.h"

SmudgeTypeClass const* const SmudgeTypeClass::Pointers[SMUDGE_COUNT] = {nullptr};
InfantryTypeClass const* const InfantryTypeClass::Pointers[INFANTRY_COUNT] = {nullptr};
TemplateTypeClass const* const TemplateTypeClass::Pointers[TEMPLATE_COUNT] = {nullptr};
OverlayTypeClass const* const OverlayTypeClass::Pointers[OVERLAY_COUNT] = {nullptr};

int CellClass::Spot_Index(COORDINATE) { return 0; }
int CellClass::Clear_Icon() const { return 0; }

bool DriveClass::Ok_To_Move(DirType) const { return true; }

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

void FlasherClass::Debug_Dump(MonoClass*) const {}
void OptionsClass::Fixup_Palette() const {}

InfantryType InfantryTypeClass::From_Name(char const*) { return INFANTRY_NONE; }

// A few narrow helpers may be provided elsewhere; provide minimal
// fallbacks only where necessary (and avoid duplicating arrays or
// large helpers that live in other translation units).
bool Queue_Mission(TARGET, MissionType) { return false; }
bool Queue_Mission(TARGET, MissionType, TARGET, TARGET) { return false; }
int Create_Air_Reinforcement(HouseClass*, AircraftType, int, MissionType, TARGET, TARGET) { return 0; }
