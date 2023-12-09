#include <Arduino.h>

#include "config.h"

#include <HomeSpan.h>

#include "homekit.h"
#include "solderfume.h"
#include "display.h"

#define PIN_FAN_PWM 15

void setup() {
  Serial.begin(115200);

  delay(500);

  display_setup();
  solderfume_setup();

  homeSpan.setWifiCredentials(WIFI_SSID1, WIFI_PASSWORD1);

  homekit_setup();
}

void loop() {
  solderfume_loop();
  homekit_loop();
  display_loop();

  homeSpan.poll();
}
