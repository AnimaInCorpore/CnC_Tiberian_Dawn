#pragma once

#include "target.h"
#include "type.h"
#include "techno.h"
#include "ftimer.h"
#include "radio.h"

class UnitClass;
class BuildingClass;

class FootClass :	public TechnoClass
{
	public:
		unsigned IsInitiated:1;
		unsigned IsNewNavCom:1;
		unsigned IsPlanningToLook:1;
		unsigned IsDeploying:1;
		unsigned IsFiring:1;
		unsigned IsRotating:1;
		unsigned IsDriving:1;
		unsigned IsUnloading:1;

		unsigned char const Speed;

		TARGET ArchiveTarget;
		TARGET NavCom;
		TARGET SuspendedNavCom;

		TeamClass * Team;
		unsigned char Group;
		FootClass * Member;

		FacingType Path[CONQUER_PATH_MAX];

		TCountDownTimerClass PathDelay;
		enum {PATH_DELAY=15,PATH_RETRY=10};
		int TryTryAgain;

		TCountDownTimerClass BaseAttackTimer;

		FootClass(void);
		virtual ~FootClass(void);
		FootClass(HousesType house);

		bool Basic_Path(void);

		virtual RadioMessageType Receive_Message(RadioClass * from, RadioMessageType message, long & param);
		virtual bool Can_Demolish(void) const;

		virtual COORDINATE Sort_Y(void) const;
		virtual COORDINATE Likely_Coord(void) const;

		COORDINATE Head_To_Coord(void) const {return (HeadToCoord);};
		virtual bool Start_Driver(COORDINATE &headto);
		virtual bool Stop_Driver(void);
		virtual void Assign_Destination(TARGET target);

		virtual bool Unlimbo(COORDINATE , DirType dir = DIR_N);
		virtual bool Limbo(void);
		virtual bool Mark(MarkType mark);

		virtual void Active_Click_With(ActionType action, ObjectClass * object);
		virtual void Active_Click_With(ActionType action, CELL cell);

		virtual void Stun(void);
		virtual ResultType Take_Damage(int & damage, int distance, WarheadType warhead, TechnoClass * source=0);
		virtual void Death_Announcement(TechnoClass const * source=0) const;

		virtual void Sell_Back(int control);
		virtual int Offload_Tiberium_Bail(void);
		virtual TARGET Greatest_Threat(ThreatType method) const;
		virtual void Detach(TARGET target, bool all);
		virtual void Detach_All(bool all=true);
		virtual void Assign_Mission(MissionType order);
		virtual int Mission_Enter(void);
		virtual int Mission_Move(void);
		virtual int Mission_Capture(void);
		virtual int Mission_Attack(void);
		virtual int Mission_Guard(void);
		virtual int Mission_Hunt(void);
		virtual int Mission_Timed_Hunt(void);
		virtual int Mission_Guard_Area(void);

		#ifdef CHEAT_KEYS
		virtual void Debug_Dump(MonoClass *mono) const;
		#endif

		virtual void Per_Cell_Process(bool center);
		virtual void Approach_Target(void);
		virtual void Fixup_Path(PathType *) {};
		virtual void Set_Speed(int speed);
		virtual MoveType Can_Enter_Cell(CELL , FacingType =FACING_NONE) const;
		int Optimize_Moves(PathType *path, MoveType threshhold);
		virtual void Override_Mission(MissionType mission, TARGET tarcom, TARGET navcom);
		virtual bool Restore_Mission(void);

		virtual void Code_Pointers(void);
		virtual void Decode_Pointers(void);

		CELL Safety_Point(CELL src, CELL dst, int start, int max);
		int Rescue_Mission(TARGET tarcom);

	private:
		int Passable_Cell(CELL cell, FacingType face, int threat, MoveType threshhold);
		PathType * Find_Path(CELL dest, FacingType *final_moves, int maxlen, MoveType threshhold);
		void Debug_Draw_Map(char const* txt, CELL start, CELL dest, bool pause);
		void Debug_Draw_Path(PathType *path);
		bool Follow_Edge(CELL start, CELL target, PathType *path, FacingType search, FacingType olddir, int threat, int threat_stage, int max_cells, MoveType threshhold);
		bool Register_Cell(PathType *path, CELL cell, FacingType dir, int cost, MoveType threshhold);
		bool Unravel_Loop(PathType *path, CELL &cell, FacingType &dir, int sx, int sy, int dx, int dy, MoveType threshhold);

		COORDINATE HeadToCoord;
};
