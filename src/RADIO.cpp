#include "function.h"
#include "radio.h"

char const* RadioClass::Messages[RADIO_COUNT] = {
    "hisssss",           "Roger.",                 "Come in.",
    "Over and out.",     "Requesting transport.",  "Attach to transport.",
    "I've got a delivery for you.",
    "I'm performing load/unload maneuver. Be careful.",
    "I'm clear.",
    "You are clear to unload. Driving away now.",
    "Am unable to comply.",
    "I'm starting construction now... act busy.",
    "I've finished construction. You are free.",
    "We bumped, redraw yourself please.",
    "I'm trying to load up now.",
    "May I become a passenger?",
    "Are you ready to receive shipment?",
    "Are you trying to become a passenger?",
    "Move to location X.",
    "Do you need to move?",
    "All right already. Now what?",
    "I'm a passenger now.",
    "Backup into refinery now.",
    "Run away!",
    "Tether established.",
    "Tether broken.",
    "Repair one step.",
    "Are you prepared to fight?",
    "Attack this target please.",
    "Reload one step.",
    "Take this kick! You... You...",
    "Take this punch! You... You...",
    "Fancy a little fisticuffs, eh?"};

#ifdef CHEAT_KEYS
void RadioClass::Debug_Dump(MonoClass* mono) const {
  mono->Set_Cursor(34, 5);
  mono->Printf(Messages[LastMessage]);
  if (Radio) {
    mono->Set_Cursor(50, 1);
    mono->Printf("%04X", Radio->As_Target());
  }
  MissionClass::Debug_Dump(mono);
}
#endif

RadioMessageType RadioClass::Receive_Message(RadioClass* from, RadioMessageType message,
                                             long& param) {
  LastMessage = message;

  if (from == Radio && message == RADIO_OVER_OUT) {
    MissionClass::Receive_Message(from, message, param);
    Radio_Off();
    return RADIO_ROGER;
  }

  if (message == RADIO_HELLO && Strength) {
    if (Radio == from || Radio == nullptr) {
      Radio = from;
      return RADIO_ROGER;
    }
    return RADIO_NEGATIVE;
  }

  return MissionClass::Receive_Message(from, message, param);
}

RadioMessageType RadioClass::Transmit_Message(RadioMessageType message, long& param,
                                              RadioClass* to) {
  if (!to) {
    to = Contact_With_Whom();
  }

  if (!to) {
    return RADIO_STATIC;
  }

  if (to == Radio && message == RADIO_OVER_OUT) {
    Radio = nullptr;
  }

  if (message == RADIO_HELLO) {
    Transmit_Message(RADIO_OVER_OUT);
    if (to->Receive_Message(this, message, param) == RADIO_ROGER) {
      Radio = to;
      return RADIO_ROGER;
    }
    return RADIO_NEGATIVE;
  }

  return to->Receive_Message(this, message, param);
}

bool RadioClass::Limbo(void) {
  if (!IsInLimbo) {
    Transmit_Message(RADIO_OVER_OUT);
  }
  return MissionClass::Limbo();
}

RadioMessageType RadioClass::Transmit_Message(RadioMessageType message, RadioClass* to) {
  return Transmit_Message(message, LParam, to);
}
