#pragma once

#include "mission.h"

class ObjectClass;
class TechnoClass;

class RadioClass : public MissionClass {
	private:

		RadioMessageType LastMessage;
		RadioClass * Radio;
		static char const * Messages[RADIO_COUNT];

	public:

		RadioClass(void) {Radio = 0;LastMessage = RADIO_STATIC;};
		virtual ~RadioClass(void) {};

		bool In_Radio_Contact(void) const {return (Radio != 0);};
		void Radio_Off(void) {Radio = 0;};
		TechnoClass * Contact_With_Whom(void) const {return (TechnoClass*)Radio;};

		virtual RadioMessageType Receive_Message(RadioClass * from, RadioMessageType message, long & param);
		virtual RadioMessageType Transmit_Message(RadioMessageType message, long & param, RadioClass * to=NULL);
		virtual RadioMessageType Transmit_Message(RadioMessageType message, RadioClass * to);
		#ifdef CHEAT_KEYS
		virtual void Debug_Dump(MonoClass *mono) const;
		#endif
		virtual bool Limbo(void);

		virtual void Code_Pointers(void);
		virtual void Decode_Pointers(void);
};
