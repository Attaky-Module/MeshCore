#include <Arduino.h>
#include "AttakyCoreBoard.h"

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
    _btn_cache = _buttons.readPressed();   // bit = 1 when pressed
    _btn_cache_ms = now;
  }
  // active-low: pressed -> LOW (matches a reverse=true MomentaryButton).
  return (_btn_cache & (1 << btn_id)) ? LOW : HIGH;
}
