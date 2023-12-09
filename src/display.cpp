#include <Arduino.h>

#include <Adafruit_SSD1306.h>

#include "solderfume.h"

// https://www.adafruit.com/product/938
// Monochrome 1.3" 128x64 OLED graphic display

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

static Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
static boolean is_present;

boolean display_setup() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));

    is_present = false;
    return false;
  }

  Serial.println("[display]");

  display.display();
  delay(2000);

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);
  display.print("hello");
  display.display();

  is_present = true;
  return true;
}

void display_loop() {
  if(!is_present)
    return;

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);
  display.print(fan_speed.get());
  display.setCursor(10, 50);
  display.print(auto_speed.get() ? "auto" : "manual");

  display.drawRect(20, 20, 20, 20, SSD1306_WHITE);

  display.display();
}
