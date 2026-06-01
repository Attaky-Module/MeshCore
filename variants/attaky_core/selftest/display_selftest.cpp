// Attaky Core display self-test: ST7789 240x320 TFT over its dedicated HSPI.
// Built only by env:attaky_core_displaytest. Draws a labelled test pattern so a
// bring-up operator can confirm the panel renders and that the orientation is
// correct (the panel is mounted 180deg in the case -> DISPLAY_ROTATION=3).
//
// Expected on screen (landscape): a white border, "Attaky Core" / "ST7789 OK"
// text near the top-left, and red / green / blue corner blocks. If the text is
// upside-down or mirrored, the rotation is wrong.

#include <Arduino.h>
#include <helpers/ui/ST7789LCDDisplay.h>

static ST7789LCDDisplay display;

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println();
  Serial.println("[Attaky Core] ST7789 display self-test");

  display.begin();
  Serial.println("display.begin() done");

  const int w = display.width();   // logical units (128)
  const int h = display.height();  // logical units (64)

  display.startFrame(DisplayDriver::DARK);

  // White border around the full logical extent.
  display.setColor(DisplayDriver::LIGHT);
  display.drawRect(0, 0, w, h);

  // Title text near top-left (confirms orientation is upright).
  display.setTextSize(1);
  display.setCursor(3, 3);
  display.print("Attaky Core");
  display.setCursor(3, 13);
  display.print("ST7789 OK");

  // Colour corner blocks to confirm colour + orientation.
  display.setColor(DisplayDriver::RED);
  display.fillRect(0, 0, 6, 6);          // top-left
  display.setColor(DisplayDriver::GREEN);
  display.fillRect(w - 6, 0, 6, 6);      // top-right
  display.setColor(DisplayDriver::BLUE);
  display.fillRect(0, h - 6, 6, 6);      // bottom-left

  display.endFrame();
  Serial.println("test pattern drawn");
}

void loop() {
  delay(1000);
}
