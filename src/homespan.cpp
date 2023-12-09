#include <HomeSpan.h>

#include "homekit.h"
#include "homekit_devices.h"
#include "solderfume.h"

Dirty<uint16_t> hk_fan_speed(0);
Dirty<bool> hk_enabled(0);
Dirty<bool> hk_auto_speed(true);

void homekit_setup() {
  homeSpan.enableOTA();
  homeSpan.begin(Category::AirPurifiers, "Solderflume air purifier");

  new SpanAccessory();                                                          
    new Identify("solderflume", "Homebus", "88-22-x", "SF01", "0.0.1");
    new AQ();
    new AirPurifier();
    new RgbLED();
}

void homekit_loop() {
  FastLED.show();
}

