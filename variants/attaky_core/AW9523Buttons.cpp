#include "AW9523Buttons.h"

// AW9523B register map (subset)
#define AW9523_REG_INPUT_P0   0x00
#define AW9523_REG_OUTPUT_P1  0x03
#define AW9523_REG_CONFIG_P0  0x04 // direction: 0=output, 1=input
#define AW9523_REG_CONFIG_P1  0x05
#define AW9523_REG_LEDMODE_P0 0x12 // 1=GPIO, 0=LED constant-current
#define AW9523_REG_LEDMODE_P1 0x13
#define AW9523_REG_SOFT_RESET 0x7F

void AW9523Buttons::writeReg(uint8_t reg, uint8_t val) {
  _wire->beginTransmission(_addr);
  _wire->write(reg);
  _wire->write(val);
  _wire->endTransmission();
}

uint8_t AW9523Buttons::readReg(uint8_t reg) {
  _wire->beginTransmission(_addr);
  _wire->write(reg);
  if (_wire->endTransmission() != 0) return 0xFF;
  _wire->requestFrom((int)_addr, 1);
  return _wire->available() ? _wire->read() : 0xFF;
}

bool AW9523Buttons::begin() {
  // Presence check.
  _wire->beginTransmission(_addr);
  if (_wire->endTransmission() != 0) {
    _ok = false;
    return false;
  }

  // 1. Software reset — clear any latched state from boot probes / noise.
  writeReg(AW9523_REG_SOFT_RESET, 0x00);
  delay(2);

  // 2. All pins in GPIO mode (1=GPIO). 0xFF on both banks is the safe
  //    direction; it never flips a button or CTP_RESET into LED mode.
  writeReg(AW9523_REG_LEDMODE_P0, 0xFF);
  writeReg(AW9523_REG_LEDMODE_P1, 0xFF);

  // 3. Direction.
  //    P0: all 8 buttons = INPUT (0xFF). Mandatory — POR drives P0_4..P0_7
  //        OUTPUT LOW at this address, which would read as phantom presses.
  //    P1: P1_0..P1_3 (RGB LED + CTP_RESET) = OUTPUT, P1_4..P1_7
  //        (passthrough) = INPUT -> 0xF0.
  writeReg(AW9523_REG_CONFIG_P0, 0xFF);
  writeReg(AW9523_REG_CONFIG_P1, 0xF0);

  // 4. Idle P1 outputs: LED off (active-low -> HIGH) and CTP_RESET released
  //    (HIGH). P1_0..P1_3 = 1 -> 0x0F; input bits ignored.
  writeReg(AW9523_REG_OUTPUT_P1, 0x0F);

  _ok = true;
  return true;
}

uint8_t AW9523Buttons::readPressed() {
  if (!_ok) return 0;
  uint8_t raw = readReg(AW9523_REG_INPUT_P0);
  if (raw == 0xFF) return 0; // all-high = nothing pressed (or bus error)
  return (uint8_t)~raw;      // active-low -> 1 = pressed
}
