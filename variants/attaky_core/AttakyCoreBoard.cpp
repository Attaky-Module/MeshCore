#include "AttakyCoreBoard.h"

#include <Arduino.h>

// Power_Standard-Cell MAX17048G+T10 fuel gauge (shared I2C bus).
#define MAX17048_I2C_ADDR  0x36
#define MAX17048_REG_VCELL 0x02 // 16-bit, LSb = 78.125 uV/cell

void AttakyCoreBoard::begin() {
  // ESP32Board::begin() brings up the shared I2C bus via
  // Wire.begin(PIN_BOARD_SDA, PIN_BOARD_SCL) and battery/TX-LED pins.
  ESP32Board::begin();

  // I2C bus stuck-state recovery (documented Attaky bus-hang case): reset the
  // Wire peripheral once before the first device init.
  Wire.end();
  delay(10);
  Wire.begin(PIN_BOARD_SDA, PIN_BOARD_SCL);

  _buttons.begin();
}

int AttakyCoreBoard::buttonLevel(uint8_t btn_id) {
  unsigned long now = millis();
  if (now - _btn_cache_ms >= 8) {
    _btn_cache = _buttons.readPressed(); // bit = 1 when pressed
    _btn_cache_ms = now;
  }
  // active-low: pressed -> LOW (matches a reverse=true MomentaryButton).
  return (_btn_cache & (1 << btn_id)) ? LOW : HIGH;
}

uint16_t AttakyCoreBoard::getBattMilliVolts() {
  // Read the MAX17048 VCELL register (0x02): point at the register, then
  // read 2 bytes MSB:LSB. A NACK means no Power Cell module is in the stack
  // -> return 0 (the ESP32Board "no battery" value), so the UI hides it.
  Wire.beginTransmission(MAX17048_I2C_ADDR);
  Wire.write(MAX17048_REG_VCELL);
  if (Wire.endTransmission() != 0) return 0;
  if (Wire.requestFrom((int)MAX17048_I2C_ADDR, 2) != 2) {
    while (Wire.available())
      Wire.read(); // drain any partial read
    return 0;
  }
  uint16_t raw = ((uint16_t)Wire.read() << 8) | Wire.read();
  // VCELL LSb = 78.125 uV/cell = 5/64 mV, so mV = raw * 5 / 64 (exact).
  return (uint16_t)(((uint32_t)raw * 5) / 64);
}
