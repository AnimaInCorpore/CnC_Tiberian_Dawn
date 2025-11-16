#pragma once

#include <cstring>

#include "session.h"

class PhoneEntryClass {
 public:
  enum PhoneEntryEnum : int {
    PHONE_MAX_NAME = 21,
    PHONE_MAX_NUM = 21
  };

  PhoneEntryClass() = default;
  ~PhoneEntryClass() = default;

  bool operator==(PhoneEntryClass const& other) const {
    return std::strncmp(Name, other.Name, PHONE_MAX_NAME) == 0;
  }
  bool operator!=(PhoneEntryClass const& other) const {
    return !(*this == other);
  }
  bool operator<(PhoneEntryClass const& other) const {
    return std::strncmp(Name, other.Name, PHONE_MAX_NAME) < 0;
  }
  bool operator>(PhoneEntryClass const& other) const {
    return other < *this;
  }
  bool operator<=(PhoneEntryClass const& other) const {
    return !(*this > other);
  }
  bool operator>=(PhoneEntryClass const& other) const {
    return !(*this < other);
  }

  SerialSettingsType Settings;
  char Name[PHONE_MAX_NAME] = {};
  char Number[PHONE_MAX_NUM] = {};
};
