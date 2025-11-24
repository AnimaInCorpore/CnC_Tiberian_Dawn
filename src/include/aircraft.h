#pragma once

#include "radio.h"
#include "fly.h"
#include "foot.h"
#include "target.h"
#include "ftimer.h"

class AircraftClass : public FootClass, public FlyClass
{
	public:
		AircraftTypeClass const * const Class;

		void * operator new(size_t);
		void operator delete(void *);
		operator AircraftType(void) const {return Class->Type;};
		AircraftClass(void) : Class(0) {};
		AircraftClass(AircraftType classid, HousesType house);
		virtual ~AircraftClass(void);
		virtual RTTIType What_Am_I(void) const {return RTTI_AIRCRAFT;};

		static void Init(void);
		enum {FLIGHT_LEVEL=24};

		virtual int Mission_Attack(void);
		virtual int Mission_Unload(void);
		virtual int Mission_Hunt(void);
		virtual int Mission_Retreat(void);
		virtual int Mission_Move(void);
		virtual int Mission_Enter(void);
		virtual int Mission_Guard(void);
		virtual int Mission_Guard_Area(void);

		bool  Process_Take_Off(void);
		bool  Process_Landing(void);
		int  Process_Fly_To(bool slowdown);

		virtual int Threat_Range(int control) const;
		virtual int Rearm_Delay(bool second) const;
		virtual MoveType Can_Enter_Cell(CELL cell, FacingType facing=FACING_NONE) const;
		virtual LayerType In_Which_Layer(void) const;
		virtual ObjectTypeClass const & Class_Of(void) const {return *Class;};
		virtual ActionType What_Action(ObjectClass * target) const;
		virtual ActionType What_Action(CELL cell) const;
		virtual DirType Desired_Load_Dir(ObjectClass * passenger, CELL & moveto) const;
		virtual int Pip_Count(void) const;
		TARGET  Good_Fire_Location(TARGET target) const;
		bool  Cell_Seems_Ok(CELL cell, bool landing=false) const;
		DirType  Pose_Dir(void) const;
		TARGET  Good_LZ(void) const;
		virtual DirType Fire_Direction(void) const;

		bool  Is_LZ_Clear(TARGET target) const;
		TARGET  New_LZ(TARGET oldlz) const;

		virtual COORDINATE Sort_Y(void) const;
		virtual COORDINATE Fire_Coord(int which) const;
		virtual COORDINATE Target_Coord(void) const;

		virtual bool Unlimbo(COORDINATE , DirType facing = DIR_N);

		virtual bool Exit_Object(TechnoClass *);
		virtual bool Mark(MarkType mark=MARK_CHANGE);
		virtual short const * Overlap_List(void) const;
		virtual void Draw_It(int x, int y, WindowNumberType window);
		virtual void Set_Speed(int speed);

		virtual void Active_Click_With(ActionType action, ObjectClass * object);
		virtual void Active_Click_With(ActionType action, CELL cell);
		virtual void Player_Assign_Mission(MissionType mission, TARGET target=TARGET_NONE, TARGET destination=TARGET_NONE);
		virtual void Response_Select(void);
		virtual void Response_Move(void);
		virtual void Response_Attack(void);

		virtual ResultType Take_Damage(int & damage, int distance, WarheadType warhead, TechnoClass * source);
		virtual BulletClass * Fire_At(TARGET target, int which);
		virtual TARGET As_Target(void) const;

		virtual void AI(void);
		virtual void Enter_Idle_Mode(bool initial = false);
		virtual RadioMessageType Receive_Message(RadioClass * from, RadioMessageType message, long & param);
		virtual void Scatter(COORDINATE threat, bool forced=false);

		#ifdef CHEAT_KEYS
		virtual void Debug_Dump(MonoClass *mono) const;
		#endif

		static void  Read_INI(char *buffer);
		static void  Write_INI(char *buffer);
		static constexpr const char *INI_Name(void) {return "AIRCRAFT";};
		bool  Load(FileClass & file);
		bool  Save(FileClass & file);
		virtual void Code_Pointers(void);
		virtual void Decode_Pointers(void);

		int Validate(void) const;

	public:

		FacingClass SecondaryFacing;

	private:

		unsigned IsLanding:1;
		unsigned IsTakingOff:1;
		unsigned IsHoming:1;
		unsigned IsHovering:1;
		unsigned char Jitter;

	public:
		int Altitude;

	private:

		TCountDownTimerClass SightTimer;
		char AttacksRemaining;
		static void * VTable;
};
