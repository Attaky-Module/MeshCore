// Attaky Core battery self-test: read the Power_Standard-Cell MAX17048G+T10
// fuel gauge over the shared I2C bus and print VCELL (mV) + SOC (%). Built
// only by env:attaky_core_batterytest. A failed read means no Power Cell
// module is in the stack (the gauge lives on the Power module, not the Core).

#include <Arduino.h>
#include <Wire.h>

#ifndef PIN_BOARD_SDA
#define PIN_BOARD_SDA 2
#endif
#ifndef PIN_BOARD_SCL
#define PIN_BOARD_SCL 1
#endif

#define MAX17048_I2C_ADDR  0x36
#define MAX17048_REG_VCELL 0x02 // 16-bit, LSb = 78.125 uV/cell
#define MAX17048_REG_SOC   0x04 // upper byte = 1%/LSb

static bool readReg16(uint8_t reg, uint16_t &out) {
  Wire.beginTransmission(MAX17048_I2C_ADDR);
  Wire.write(reg);
  if (Wire.endTransmission() != 0) return false;
  if (Wire.requestFrom((int)MAX17048_I2C_ADDR, 2) != 2) {
    while (Wire.available())
      Wire.read(); // drain any partial read
    return false;
  }
  out = ((uint16_t)Wire.read() << 8) | Wire.read();
  return true;
}

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println();
  Serial.println("[Attaky Core] MAX17048 battery self-test");
  // Shared I2C bus stuck-state recovery, mirroring AttakyCoreBoard::begin().
  Wire.end();
  delay(10);
  Wire.begin(PIN_BOARD_SDA, PIN_BOARD_SCL);
}

void loop() {
  uint16_t vcell, soc;
  if (!readReg16(MAX17048_REG_VCELL, vcell)) {
    Serial.println("VCELL read failed (no Power Cell module on I2C 0x36?)");
  } else {
    uint16_t mv = (uint16_t)(((uint32_t)vcell * 5) / 64); // 78.125 uV/LSb
    if (readReg16(MAX17048_REG_SOC, soc)) {
      Serial.printf("VCELL raw=%u mV=%u  SOC=%u%%\n", vcell, mv, soc >> 8);
    } else {
      Serial.printf("VCELL raw=%u mV=%u  SOC=?\n", vcell, mv);
    }
  }
  delay(1000);
}
