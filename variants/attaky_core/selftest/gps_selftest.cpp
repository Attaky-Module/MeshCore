// Attaky Core GPS UART self-test: echo raw NMEA from the Expand_LoRa-GPS
// ATGM336H. Built only by env:attaky_core_gpstest. Confirms the GPS UART
// wiring + that the module is alive: NMEA sentences ($GxGGA/$GxRMC/...) stream
// even indoors (with empty fix fields) when the wiring is correct. If nothing
// arrives, the TX/RX mapping or the module is wrong.
//
// Uses MeshCore's pin convention: Serial1.setPins(PIN_GPS_TX, PIN_GPS_RX),
// where setPins(rx, tx) makes PIN_GPS_TX the ESP RX pin (IO18 <- GPS TXD).

#include <Arduino.h>

#ifndef PIN_GPS_TX
#define PIN_GPS_TX 18
#endif
#ifndef PIN_GPS_RX
#define PIN_GPS_RX 17
#endif
#ifndef GPS_BAUD_RATE
#define GPS_BAUD_RATE 9600
#endif

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println();
  Serial.println("[Attaky Core] GPS UART self-test (raw NMEA echo)");
  // Explicit pins in begin() (rx=PIN_GPS_TX=IO18 <- GPS TXD, tx=PIN_GPS_RX=IO17)
  // to rule out setPins-before-begin not sticking on ESP32-S3.
  Serial1.begin(GPS_BAUD_RATE, SERIAL_8N1, PIN_GPS_TX, PIN_GPS_RX);
  Serial.println("Listening on IO18 for NMEA; expect $GxGGA/$GxRMC lines...");
}

void loop() {
  while (Serial1.available()) {
    Serial.write(Serial1.read());
  }
}
