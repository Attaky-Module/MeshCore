#pragma once

#include <Wire.h>
#include <Arduino.h>
#include "helpers/ESP32Board.h"
#include "AW9523Buttons.h"

// Attaky Core (ESP32-S3-WROOM-1U-N16R8) main board.
//
// Shared I2C bus (SDA=IO2, SCL=IO1) carries the two AW9523 GPIO expanders
// (@0x58 / @0x59), the QMI8658 IMU, the STK3311 ALS and the FT6636 touch IC.
// Wire.begin(PIN_BOARD_SDA, PIN_BOARD_SCL) is handled by ESP32Board::begin().
//
// The board owns the @0x59 button expander and exposes buttonLevel() so the
// UI's MomentaryButton instances can be backed by the face buttons.
class AttakyCoreBoard : public ESP32Board {
  AW9523Buttons _buttons;
  uint8_t       _btn_cache = 0;
  unsigned long _btn_cache_ms = 0;

public:
  AttakyCoreBoard() : _buttons(0x59) {}

  void begin();

  const char* getManufacturerName() const override {
    return "Attaky Core";
  }

  // Raw digital level (LOW = pressed, active-low) for an ATTAKY_BTN_* id, for
  // use as a MomentaryButton read provider. The AW9523 P0 byte is cached for a
  // few ms so a UI loop polling several buttons triggers only one I2C read.
  int buttonLevel(uint8_t btn_id);

  // Battery voltage from the Power_Standard-Cell MAX17048 fuel gauge over the
  // shared I2C bus. Returns 0 when no Power Cell module is in the stack (the
  // ESP32Board default for "no battery"), since the Core has no ADC divider.
  uint16_t getBattMilliVolts() override;
};
