#pragma once

#include "object.h"
#include "monoc.h"
#include "ftimer.h"

class MissionClass : public ObjectClass
{
	public:

		MissionType Mission;
		MissionType SuspendedMission;
		MissionType MissionQueue;

		char Status;

		MissionClass(void);
		virtual ~MissionClass(void) {};

		#ifdef CHEAT_KEYS
		void  Debug_Dump(MonoClass *mono) const;
		#endif

		virtual MissionType Get_Mission(void) const;
		virtual void  Assign_Mission(MissionType mission);
		virtual bool Commence(void);
		virtual void AI(void);

		virtual int Mission_Sleep(void);
		virtual int Mission_Ambush(void);
		virtual int Mission_Attack(void);
		virtual int Mission_Capture(void);
		virtual int Mission_Guard(void);
		virtual int Mission_Guard_Area(void);
		virtual int Mission_Harvest(void);
		virtual int Mission_Hunt(void);
		virtual int Mission_Timed_Hunt(void);
		virtual int Mission_Move(void);
		virtual int Mission_Retreat(void);
		virtual int Mission_Return(void);
		virtual int Mission_Stop(void);
		virtual int Mission_Unload(void);
		virtual int Mission_Enter(void);
		virtual int Mission_Construction(void);
		virtual int Mission_Deconstruction(void);
		virtual int Mission_Repair(void);
		virtual int Mission_Missile(void);
		virtual void  Set_Mission(MissionType mission);

		static char const *  Mission_Name(MissionType order);
		static MissionType  Mission_From_Name(char const *name);
		virtual void  Override_Mission(MissionType mission, TARGET, TARGET);
		virtual bool Restore_Mission(void);

		virtual void Code_Pointers(void);
		virtual void Decode_Pointers(void);

	private:
		TCountDownTimerClass Timer;
		static char const * Missions[MISSION_COUNT];
};
