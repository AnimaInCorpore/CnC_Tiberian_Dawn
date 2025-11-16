#pragma once

#include <cstdint>

class LEDClass {
 public:
  enum ControlType : std::uint8_t {
    LED_NOCHANGE,  // Do nothing (just query).
    LED_OFF,       // Turn LED off.
    LED_ON,        // Turn LED on.
    LED_TOGGLE     // Toggle LED state.
  };

 protected:
  static int Shift_Control(ControlType control, std::uint8_t bit);

 public:
  static int Scroll_Lock(ControlType control = LED_TOGGLE) {
    return Shift_Control(control, 0x01);
  }
  static int Caps_Lock(ControlType control = LED_TOGGLE) {
    return Shift_Control(control, 0x02);
  }
  static int Num_Lock(ControlType control = LED_TOGGLE) {
    return Shift_Control(control, 0x04);
  }

 private:
  static void Send_To_Keyboard(std::uint8_t value);
};
