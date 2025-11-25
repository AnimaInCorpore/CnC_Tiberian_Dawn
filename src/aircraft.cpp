#include "aircraft.h"
#include "function.h"
#include "mapedit.h"

#include <cstdio>
#include <cstring>

void * AircraftClass::VTable;

#ifdef CHEAT_KEYS
int AircraftClass::Validate(void) const
{
	int num;

	num = Aircraft.ID(this);
	if (num < 0 || num >= AIRCRAFT_MAX) {
		Validate_Error("AIRCRAFT");
		return (0);
	}
	else
		return (1);
}
#else
#define	Validate()
#endif

TARGET AircraftClass::As_Target(void) const
{
	Validate();
	return(Build_Target(KIND_AIRCRAFT, Aircraft.ID(this)));
}

void * AircraftClass::operator new(size_t)
{
	void * ptr = Aircraft.Allocate();
	if (ptr) {
		((AircraftClass *)ptr)->IsActive = true;
	}
	return(ptr);
}

void AircraftClass::operator delete(void *ptr)
{
	if (ptr) {
		((AircraftClass *)ptr)->IsActive = false;
	}
	Aircraft.Free((AircraftClass *)ptr);
}

AircraftClass::AircraftClass(AircraftType classid, HousesType house) :
	Class(&AircraftTypeClass::As_Reference(classid)),
	FootClass(house)
{
	if (Class->IsTwoShooter) {
		IsSecondShot = false;
	} else {
		IsSecondShot = true;
	}
	Ammo = Class->MaxAmmo;
	AttacksRemaining = 3;
	Altitude = FLIGHT_LEVEL;
	IsLanding = false;
	IsTakingOff = false;
	IsHovering = false;
	IsHoming = false;
	Strength = Class->MaxStrength;
	NavCom = TARGET_NONE;
	SecondaryFacing = PrimaryFacing;
	Jitter = 0;

	if (classid != AIRCRAFT_CARGO && GameToPlay == GAME_INTERNET){
		House->AircraftTotals->Increment_Unit_Total((int)classid);
	}
}

bool AircraftClass::Unlimbo(COORDINATE coord, DirType dir)
{
	Validate();
	if (FootClass::Unlimbo(coord, dir)) {

		House->AScan |= (1L << Class->Type);
		House->ActiveAScan |= (1L << Class->Type);

		SecondaryFacing = dir;

		Set_Rate(1);
		Set_Stage(0);

		if (Altitude == FLIGHT_LEVEL) {
			Set_Speed(0xFF);
		} else {
			Set_Speed(0);
		}
		return(true);
	}
	return(false);
}

void AircraftClass::Draw_It(int x, int y, WindowNumberType window)
{
	Validate();
	void const * shapefile;
	int shapenum = 0;
	int facing = Facing_To_32(SecondaryFacing);

	shapefile = Class->Get_Image_Data();
	if (!shapefile) return;
	shapenum = UnitClass::BodyShape[facing];

	if (*this == AIRCRAFT_ORCA && Get_Speed() >= MPH_MEDIUM_FAST) {
		shapenum += 32;
	}

	if (!Is_Door_Closed()) {
		shapenum = 32 + Door_Stage();
	}

	int jitter = 0;
	if (Altitude == FLIGHT_LEVEL && !Class->IsFixedWing) {
		Jitter++;

		static int _jitter[] = {0,0,0,0,1,1,1,0,0,0,0,0,-1,-1,-1,0};
		jitter = _jitter[Jitter % 16];
	}

	if (Visual_Character() <= VISUAL_DARKEN) {
		CC_Draw_Shape(shapefile, shapenum, x+1, y+2, window, SHAPE_PREDATOR|SHAPE_CENTER|SHAPE_WIN_REL|SHAPE_FADING, Map.FadingShade, NULL);
	}

	Techno_Draw_Object(shapefile, shapenum, x, (y-Altitude)+jitter, window);

	if (Class->IsRotorEquipped) {
		ShapeFlags_Type flags = SHAPE_CENTER|SHAPE_WIN_REL;

		if (Altitude == 0) {
			shapenum = (Fetch_Stage()%8)+4;
			flags = flags | SHAPE_GHOST;
		} else {
			shapenum = Fetch_Stage()%4;
			flags = flags | SHAPE_FADING|SHAPE_PREDATOR;
		}

		if (*this == AIRCRAFT_TRANSPORT) {
			int _stretch[FACING_COUNT] = {8, 9, 10, 9, 8, 9, 10, 9};

			short xx = x;
			short yy = y-Altitude;
			FacingType face = Dir_Facing(SecondaryFacing);
			Move_Point(xx, yy, SecondaryFacing.Current(), _stretch[face]);
			CC_Draw_Shape(Class->RRotorData, shapenum, xx, yy-2, window, flags, NULL, Map.UnitShadow);

			Move_Point(xx, yy, SecondaryFacing.Current()+DIR_S, _stretch[face]*2);
			CC_Draw_Shape(Class->LRotorData, shapenum, xx, yy-2, window, flags, NULL, Map.UnitShadow);

		} else {
			CC_Draw_Shape(Class->RRotorData, shapenum, x, (y-Altitude)-2, window, flags, NULL, Map.UnitShadow);
		}
	}

	FootClass::Draw_It(x, y-Altitude, window);
}

void AircraftClass::Read_INI(char *buffer)
{
	AircraftClass	*air;
	char			*tbuffer;
	HousesType		inhouse;
	AircraftType	classid;
	int			len;
	char			buf[128];

	len = strlen(buffer) + 2;
	tbuffer = buffer + len;

	WWGetPrivateProfileString(INI_Name(), NULL, NULL, tbuffer, ShapeBufferSize-len, buffer);
	while (*tbuffer != '\0') {

		WWGetPrivateProfileString(INI_Name(), tbuffer, NULL, buf, sizeof(buf)-1, buffer);
		inhouse = HouseTypeClass::From_Name(strtok(buf, ","));
		if (inhouse != HOUSE_NONE) {
			classid = AircraftTypeClass::From_Name(strtok(NULL, ","));

			if (classid != AIRCRAFT_NONE) {

				air = new AircraftClass(classid, inhouse);
				if (air) {
					COORDINATE	coord;
					int	strength;
					DirType dir;

					strength = atoi(strtok(NULL, ","));
					coord = Cell_Coord((CELL)atoi(strtok(NULL, ",")));
					dir = (DirType)atoi(strtok(NULL, ","));

					if (!Map.In_Radar(Coord_Cell(coord))) {
						delete air;
					} else {

						air->Strength = Fixed_To_Cardinal(air->Class->MaxStrength, strength);
						if (air->Unlimbo(coord, dir)) {
							air->Assign_Mission(AircraftClass::Mission_From_Name(strtok(NULL, "\n\r")));
						} else {
							delete air;
						}
					}
				}
			}
		}
		tbuffer += strlen(tbuffer)+1;
	}
}

void AircraftClass::Write_INI(char *buffer)
{
	int	index;
	char	uname[10];
	char	buf[128];
	char	*tbuffer;

	tbuffer = buffer + strlen(buffer) + 2;
	WWGetPrivateProfileString(INI_Name(), nullptr, nullptr, tbuffer,
	                          ShapeBufferSize - strlen(buffer), buffer);
	while (*tbuffer != '\0') {
		WWWritePrivateProfileString(INI_Name(), tbuffer, nullptr, buffer);
		tbuffer += strlen(tbuffer)+1;
	}

	for (index = 0; index < Aircraft.Count(); index++) {
		AircraftClass * unit;

		unit = Aircraft.Ptr(index);
		if (!unit->IsInLimbo) {

			std::snprintf(uname, sizeof(uname), "%03d", index);
			std::snprintf(buf, sizeof(buf), "%s,%s,%d,%u,%d,%s",
				unit->House->Class->IniName,
				unit->Class->IniName,
				unit->Health_Ratio(),
				Coord_Cell(unit->Coord),
				unit->PrimaryFacing.Current(),
				MissionClass::Mission_Name(unit->Mission)
				);
			WWWritePrivateProfileString(INI_Name(), uname, buf, buffer);
		}
	}
}

// --- Stubbed behavior for unported routines -------------------------------------------------

AircraftClass::~AircraftClass() = default;

int AircraftClass::Mission_Attack() { return 0; }
int AircraftClass::Mission_Unload() { return 0; }
int AircraftClass::Mission_Hunt() { return 0; }
int AircraftClass::Mission_Retreat() { return 0; }
int AircraftClass::Mission_Move() { return 0; }
int AircraftClass::Mission_Enter() { return 0; }
int AircraftClass::Mission_Guard() { return 0; }
int AircraftClass::Mission_Guard_Area() { return 0; }
int AircraftClass::Threat_Range(int) const { return 0; }
int AircraftClass::Rearm_Delay(bool) const { return 0; }
MoveType AircraftClass::Can_Enter_Cell(CELL, FacingType) const { return MOVE_OK; }
LayerType AircraftClass::In_Which_Layer() const { return LAYER_GROUND; }
ActionType AircraftClass::What_Action(ObjectClass*) const { return ACTION_NONE; }
ActionType AircraftClass::What_Action(CELL) const { return ACTION_NONE; }
DirType AircraftClass::Desired_Load_Dir(ObjectClass*, CELL&) const { return DIR_N; }
int AircraftClass::Pip_Count() const { return 0; }
TARGET AircraftClass::Good_Fire_Location(TARGET) const { return 0; }
bool AircraftClass::Cell_Seems_Ok(CELL, bool) const { return true; }
DirType AircraftClass::Pose_Dir() const { return DIR_N; }
TARGET AircraftClass::Good_LZ() const { return 0; }
DirType AircraftClass::Fire_Direction() const { return DIR_N; }
bool AircraftClass::Is_LZ_Clear(TARGET) const { return true; }
TARGET AircraftClass::New_LZ(TARGET) const { return 0; }
COORDINATE AircraftClass::Sort_Y() const { return Coord; }
COORDINATE AircraftClass::Fire_Coord(int) const { return Coord; }
COORDINATE AircraftClass::Target_Coord() const { return Coord; }
int AircraftClass::Exit_Object(TechnoClass*) { return 0; }
bool AircraftClass::Mark(MarkType) { return true; }
short const* AircraftClass::Overlap_List() const { return nullptr; }
void AircraftClass::Set_Speed(int) {}
void AircraftClass::Active_Click_With(ActionType, ObjectClass*) {}
void AircraftClass::Active_Click_With(ActionType, CELL) {}
void AircraftClass::Player_Assign_Mission(MissionType, TARGET, TARGET) {}
void AircraftClass::Response_Select() {}
void AircraftClass::Response_Move() {}
void AircraftClass::Response_Attack() {}
bool AircraftClass::Process_Landing() { return true; }
int AircraftClass::Process_Fly_To(bool) { return 0; }
ResultType AircraftClass::Take_Damage(int& damage, int, WarheadType, TechnoClass*) {
	damage = 0;
	return RESULT_NONE;
}
void AircraftClass::Code_Pointers() {}
void AircraftClass::Decode_Pointers() {}
void AircraftClass::Enter_Idle_Mode(bool) {}
RadioMessageType AircraftClass::Receive_Message(RadioClass*, RadioMessageType message, long&) { return message; }
void AircraftClass::AI() {}
BulletClass* AircraftClass::Fire_At(TARGET, int) { return nullptr; }
void AircraftClass::Scatter(COORDINATE, bool) {}
#ifdef CHEAT_KEYS
void AircraftClass::Debug_Dump(MonoClass*) const {}
#endif
