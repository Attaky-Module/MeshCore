// Attaky Core board self-test: AW9523 @0x59 button read-out over serial.
// Built only by env:attaky_core_buttontest. Prints PRESS/RELEASE events so a
// bring-up operator can confirm each face button maps to the right key and
// that LEFT/RIGHT are not swapped.
//
// NOTE: POWER is a hardware power toggle on long-press (handled by the PMIC,
// not interceptable). Short-press it only during this test.

#include "../AW9523Buttons.h"

#include <Arduino.h>
#include <Wire.h>

#ifndef PIN_BOARD_SDA
#define PIN_BOARD_SDA 2
#endif
#ifndef PIN_BOARD_SCL
#define PIN_BOARD_SCL 1
#endif

static AW9523Buttons buttons(0x59);
static uint8_t last_state = 0;

static const char *BTN_NAMES[8] = { "DOWN", "LEFT", "SELECT", "RIGHT", "UP", "L1", "R2", "POWER" };

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println();
  Serial.println("[Attaky Core] AW9523 @0x59 button self-test");
  Serial.println("Press each button; expect the matching name. Short-press POWER only.");

  // I2C bus stuck-state recovery (documented Attaky bus-hang case): reset the
  // Wire peripheral once before init in case a prior session left it hung.
  Wire.begin(PIN_BOARD_SDA, PIN_BOARD_SCL);
  Wire.end();
  delay(10);
  Wire.begin(PIN_BOARD_SDA, PIN_BOARD_SCL);

  bool ok = buttons.begin();
  Serial.print("AW9523 @0x59 init: ");
  Serial.println(ok ? "OK" : "NOT FOUND");
}

void loop() {
  uint8_t now = buttons.readPressed();
  if (now != last_state) {
    for (int b = 0; b < 8; b++) {
      uint8_t mask = (uint8_t)(1 << b);
      if ((now & mask) && !(last_state & mask)) {
        Serial.print("PRESS   ");
        Serial.println(BTN_NAMES[b]);
      } else if (!(now & mask) && (last_state & mask)) {
        Serial.print("RELEASE ");
        Serial.println(BTN_NAMES[b]);
      }
    }
    last_state = now;
  }
  delay(20);
}
