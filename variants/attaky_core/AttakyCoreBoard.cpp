#include <Arduino.h>
#include "AttakyCoreBoard.h"

void AttakyCoreBoard::begin() {
  // ESP32Board::begin() brings up the shared I2C bus via
  // Wire.begin(PIN_BOARD_SDA, PIN_BOARD_SCL) and battery/TX-LED pins.
  ESP32Board::begin();

  // Batch 2+: AW9523 @0x59 button scan + RGB LED, MAX17048 battery, etc.
}
