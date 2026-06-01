#pragma once

#include <Wire.h>
#include <Arduino.h>
#include "helpers/ESP32Board.h"

// Attaky Core (ESP32-S3-WROOM-1U-N16R8) main board.
//
// Shared I2C bus (SDA=IO2, SCL=IO1) carries the two AW9523 GPIO expanders
// (@0x58 / @0x59), the QMI8658 IMU, the STK3311 ALS and the FT6636 touch IC.
// Wire.begin(PIN_BOARD_SDA, PIN_BOARD_SCL) is handled by ESP32Board::begin().
//
// Batch 0/1: minimal board (radio only). Later batches add:
//   - Batch 2: AW9523 @0x59 D-pad/button input + RGB LED
//   - Batch 8: MAX17048 fuel-gauge battery reporting (Power_Standard-Cell)
class AttakyCoreBoard : public ESP32Board {
public:
  void begin();

  const char* getManufacturerName() const override {
    return "Attaky Core";
  }
};
