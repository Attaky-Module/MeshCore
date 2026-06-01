#include <Arduino.h>
#include "target.h"

AttakyCoreBoard board;

#if defined(P_LORA_SCLK)
  static SPIClass spi;
  RADIO_CLASS radio = new Module(P_LORA_NSS, P_LORA_DIO_1, P_LORA_RESET, P_LORA_BUSY, spi);
#else
  RADIO_CLASS radio = new Module(P_LORA_NSS, P_LORA_DIO_1, P_LORA_RESET, P_LORA_BUSY);
#endif

WRAPPER_CLASS radio_driver(radio, board);

ESP32RTCClock fallback_clock;
AutoDiscoverRTCClock rtc_clock(fallback_clock);

#if ENV_INCLUDE_GPS
  // Expand_LoRa-GPS ATGM336H on Serial1 (host RX=IO18 <- GPS TXD, TX=IO17).
  MicroNMEALocationProvider nmea(Serial1, &rtc_clock);
  EnvironmentSensorManager sensors(nmea);
#else
  EnvironmentSensorManager sensors;
#endif

#ifdef DISPLAY_CLASS
  DISPLAY_CLASS display;

  // AW9523-backed read provider for the face buttons (active-low -> LOW=pressed).
  static int attaky_btn_level(uint8_t btn_id) { return board.buttonLevel(btn_id); }

  // UI_HAS_JOYSTICK nav mapping (consumed by UITask): user_btn -> KEY_ENTER,
  // left -> KEY_LEFT, right -> KEY_RIGHT, back -> (triple) KEY_SELECT. Physical
  // SELECT drives user_btn because HomeScreen actions are on KEY_ENTER.
  MomentaryButton user_btn(ATTAKY_BTN_SELECT, 1000, true, false, false, attaky_btn_level);
  MomentaryButton joystick_left(ATTAKY_BTN_LEFT, 1000, true, false, false, attaky_btn_level);
  MomentaryButton joystick_right(ATTAKY_BTN_RIGHT, 1000, true, false, false, attaky_btn_level);
  MomentaryButton back_btn(ATTAKY_BTN_L1, 1000, true, false, true, attaky_btn_level);
#endif

#ifndef LORA_CR
  #define LORA_CR  5
#endif

bool radio_init() {
  fallback_clock.begin();
  rtc_clock.begin(Wire);

#ifdef SX126X_DIO3_TCXO_VOLTAGE
  float tcxo = SX126X_DIO3_TCXO_VOLTAGE;   // Ra-01SH: 0 = crystal (no TCXO)
#else
  float tcxo = 1.6f;
#endif

#if defined(P_LORA_SCLK)
  spi.begin(P_LORA_SCLK, P_LORA_MISO, P_LORA_MOSI);
#endif
  int status = radio.begin(LORA_FREQ, LORA_BW, LORA_SF, LORA_CR, RADIOLIB_SX126X_SYNC_WORD_PRIVATE, LORA_TX_POWER, 8, tcxo);
  if (status != RADIOLIB_ERR_NONE) {
    Serial.print("ERROR: radio init failed: ");
    Serial.println(status);
    return false;  // fail
  }

  radio.setCRC(1);

#ifdef SX126X_CURRENT_LIMIT
  radio.setCurrentLimit(SX126X_CURRENT_LIMIT);
#endif
#ifdef SX126X_DIO2_AS_RF_SWITCH
  radio.setDio2AsRfSwitch(SX126X_DIO2_AS_RF_SWITCH);
#endif
#ifdef SX126X_RX_BOOSTED_GAIN
  radio.setRxBoostedGainMode(SX126X_RX_BOOSTED_GAIN);
#endif

  return true;  // success
}

mesh::LocalIdentity radio_new_identity() {
  RadioNoiseListener rng(radio);
  return mesh::LocalIdentity(&rng);  // create new random identity
}
