#pragma once

#include <Arduino.h>
#include <Wire.h>

// AW9523B @0x59 on the Attaky Core shared I2C bus (SDA=IO2, SCL=IO1).
// Bank P0 (P0_0..P0_7) carries the 8 face buttons (active-low, external
// pull-ups). Bank P1 carries the RGB LED (P1_0..P1_2), the touch-controller
// reset (P1_3 = CTP_RESET) and passthrough lines (P1_4..P1_7).
//
// This driver reads the P0 buttons. It puts the LED pins in their idle
// (off) state and releases CTP_RESET, but does not otherwise drive bank P1.
//
// POR gotcha: at address 0x59 the AW9523 powers up driving P0_4..P0_7
// OUTPUT LOW. Without an explicit input-direction write those four lines
// read LOW and look like UP/L1/R2/POWER are stuck pressed. begin() issues a
// software reset and sets every button pin to INPUT to avoid this.
//
// Button bit map (P0 bit -> key), per Attaky Core host pinmap v0.9:
#define ATTAKY_BTN_DOWN   0 // P0_0
#define ATTAKY_BTN_LEFT   1 // P0_1
#define ATTAKY_BTN_SELECT 2 // P0_2
#define ATTAKY_BTN_RIGHT  3 // P0_3
#define ATTAKY_BTN_UP     4 // P0_4
#define ATTAKY_BTN_L1     5 // P0_5
#define ATTAKY_BTN_R2     6 // P0_6
#define ATTAKY_BTN_POWER  7 // P0_7

class AW9523Buttons {
public:
  explicit AW9523Buttons(uint8_t addr = 0x59, TwoWire *wire = &Wire) : _addr(addr), _wire(wire), _ok(false) {}

  // Canonical AW9523 init. Wire.begin() must already have run.
  // Returns true if the chip ACKs on the bus.
  bool begin();

  // Pressed bitmask over bank P0: bit N (ATTAKY_BTN_*) is 1 when that
  // button is pressed (line pulled LOW). Returns 0 on bus error.
  uint8_t readPressed();

  bool isReady() const { return _ok; }

private:
  uint8_t _addr;
  TwoWire *_wire;
  bool _ok;

  void writeReg(uint8_t reg, uint8_t val);
  uint8_t readReg(uint8_t reg);
};
