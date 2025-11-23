#pragma once

#include "mission.h"

class MonoClass;
class TechnoClass;

class RadioClass : public MissionClass {
 private:
  RadioMessageType LastMessage = RADIO_STATIC;
  RadioClass* Radio = nullptr;
  static char const* Messages[RADIO_COUNT];
  static long DefaultRadioParam;

 public:
  RadioClass() = default;
  virtual ~RadioClass() = default;

  [[nodiscard]] bool In_Radio_Contact() const { return Radio != nullptr; }
  void Radio_Off() { Radio = nullptr; }
  TechnoClass* Contact_With_Whom() const { return reinterpret_cast<TechnoClass*>(Radio); }

  virtual RadioMessageType Receive_Message(RadioClass* from, RadioMessageType message,
                                           long& param);
  virtual RadioMessageType Transmit_Message(RadioMessageType message, long& param = DefaultRadioParam,
                                             RadioClass* to = nullptr);
  virtual RadioMessageType Transmit_Message(RadioMessageType message, RadioClass* to);
#ifdef CHEAT_KEYS
  virtual void Debug_Dump(MonoClass* mono) const;
#endif
  virtual bool Limbo();

  virtual void Code_Pointers();
  virtual void Decode_Pointers();
};
