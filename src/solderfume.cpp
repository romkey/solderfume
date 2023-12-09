#include "solderfume.h"

#include <Arduino.h>
#include <Wire.h>
#include <time.h>

#include "config.h"

#include "dirty.h"

#include <Adafruit_PM25AQI.h>
#include <Adafruit_EMC2101.h>

#include "solderfume.h"

Adafruit_EMC2101  emc2101;
Adafruit_PM25AQI  pm;

Dirty<float> pm10_value(0);
Dirty<float> pm25_value(0);
Dirty<uint16_t> fan_speed(0);
Dirty<bool> enabled(true);
Dirty<bool> auto_speed(true);

void solderfume_setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("solderflume");

  //  Wire.begin(3, 5);

  if(!emc2101.begin()) {
    Serial.println("Failed to find EMC2101 chip");
    while (1) { delay(10); }
  }

  emc2101.enableTachInput(true);
  emc2101.setPWMDivisor(0);
  emc2101.setDutyCycle(0);
  emc2101.setFanMinRPM(100);

  Serial.println("[emc2101]");

  if(!pm.begin_I2C()) {
    Serial.println("Could not find PM 2.5 sensor!");
    while (1) delay(10);
  }

  Serial.println("[pma5003]");
}

void solderfume_clean1() {
    enabled.clean();
    fan_speed.clean();
}

boolean CHECKIT = false;

void solderfume_loop() {
  static uint64_t next_loop = 0;

  if(CHECKIT) {
    Serial.println("CHECKIT");

    hk_enabled.dump("enabled");
    hk_fan_speed.dump("fan_speed");
    hk_auto_speed.dump("auto_speed");

    CHECKIT = false;
  }

  // did we get turned off?
  if(hk_enabled.dirty) {
    Serial.println("enabled dirty!");

    if(hk_enabled.get()) {
      Serial.println("turned ON!");
      auto_speed.set(true);
    } else {
      Serial.println("turned OFF!");
      emc2101.setDutyCycle(0);
    }
  }

  if(hk_fan_speed.dirty) {
    Serial.println("FAN SPEED DIRTY");

    Serial.println("auto OFF");
    auto_speed.set(false);

    Serial.print("fan_speed ");
    Serial.println((hk_fan_speed.get()/100.0)*1900);
    emc2101.setDutyCycle(hk_fan_speed.get());
  }

  if(enabled.get() && auto_speed.get()) {
    if(pm10_value.dirty || pm25_value.dirty) {
      uint8_t percent = constrain(map(constrain(max(pm10_value.get(), pm25_value.get()), 0, 200), 0, 200, 0, 100), 0, 100);
      Serial.print("auto fan_speed %");
      Serial.println(percent);
      emc2101.setDutyCycle(percent);
      fan_speed.set(percent);
    }
  }

  hk_auto_speed.clean();
  hk_fan_speed.clean();
  hk_enabled.clean();

  if(next_loop > millis())
    return;

  next_loop = millis() + UPDATE_INTERVAL;

  PM25_AQI_Data data;
  
  if(!pm.read(&data)) {
    Serial.println("Could not read from AQI");
    return;
  }

  Serial.println(F("Concentration Units (standard)"));
  Serial.println(F("---------------------------------------"));
  Serial.print(F("PM 1.0: ")); Serial.print(data.pm10_standard);
  Serial.print(F("\t\tPM 2.5: ")); Serial.print(data.pm25_standard);
  Serial.print(F("\t\tPM 10: ")); Serial.println(data.pm100_standard);
  Serial.println(F("Concentration Units (environmental)"));
  Serial.println(F("---------------------------------------"));
  Serial.print(F("PM 1.0: ")); Serial.print(data.pm10_env);
  Serial.print(F("\t\tPM 2.5: ")); Serial.print(data.pm25_env);
  Serial.print(F("\t\tPM 10: ")); Serial.println(data.pm100_env);
  Serial.println(F("---------------------------------------"));
  Serial.print(F("Particles > 0.3um / 0.1L air:")); Serial.println(data.particles_03um);
  Serial.print(F("Particles > 0.5um / 0.1L air:")); Serial.println(data.particles_05um);
  Serial.print(F("Particles > 1.0um / 0.1L air:")); Serial.println(data.particles_10um);
  Serial.print(F("Particles > 2.5um / 0.1L air:")); Serial.println(data.particles_25um);
  Serial.print(F("Particles > 5.0um / 0.1L air:")); Serial.println(data.particles_50um);
  Serial.print(F("Particles > 10 um / 0.1L air:")); Serial.println(data.particles_100um);

  Serial.print("Internal Temperature: ");
  Serial.print(emc2101.getInternalTemperature());
  Serial.println(" degrees C");

  Serial.print("Duty Cycle: ");
  Serial.print(emc2101.getDutyCycle());
  Serial.print("% / Fan RPM: ");
  Serial.print(emc2101.getFanRPM());
  Serial.println(" RPM");
  Serial.println("");  

  pm25_value.set(data.pm25_standard);
  pm10_value.set(data.pm100_standard);
}
