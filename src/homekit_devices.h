#pragma once

#include <HomeSpan.h>
#include <FastLED.h>

#include "dirty.h"
#include "solderfume.h"

struct Identify : Service::AccessoryInformation {
  int nBlinks;
  SpanCharacteristic *identify;
  
  Identify(const char *name, const char *manu, const char *sn, const char *model, const char *version) : Service::AccessoryInformation() {
    String mac_address = WiFi.macAddress();

    new Characteristic::Name(name);
    new Characteristic::Manufacturer(manu);
    new Characteristic::SerialNumber(mac_address.c_str());
    new Characteristic::Model(model);
    new Characteristic::FirmwareRevision(version);

    identify = new Characteristic::Identify();

    pinMode(LED_BUILTIN, OUTPUT);                      // make sure built-in LED is set for output
  }

  boolean update() {
    for(int i = 0 ; i< 10; i++) {
      digitalWrite(LED_BUILTIN, LOW);
      delay(250);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(250);
    }

    return(true);
  }
};

struct AQ : Service::AirQualitySensor {
  SpanCharacteristic *aq;
  SpanCharacteristic *pm25;
  SpanCharacteristic *pm10;

  // HAP spec chapter 9
  uint8_t _aq_value;  // 9-9 0: unknown, 1: excellent, 2: good, 3: fair, 4: inferior, 5: poor

  AQ() : Service::AirQualitySensor() {
    new Characteristic::Name("Solder Fume Air Quality");

    aq = new Characteristic::AirQuality;
    pm25 = new Characteristic::PM25Density;
    pm10 = new Characteristic::PM10Density;
  }

  void loop() {
    if(pm25_value.dirty || pm10_value.dirty) {
      _aq_value = min((float)1.0, max((float)100.0, max(pm25_value.get(), pm10_value.get())/20));
      aq->setVal(_aq_value);
    }

    if(pm25_value.dirty)  {
      pm25->setVal(pm25_value.get());
      pm25_value.clean();
    }

    if(pm10_value.dirty) {
      pm10->setVal(pm10_value.get());
      pm10_value.clean();
    }
  }
};
  
struct AirPurifier : Service::AirPurifier {
  SpanCharacteristic *sc_active; // ro. 9-3 0: inactive, 1: active
  SpanCharacteristic *sc_current_state; // ro, 9-25 0: inactive, 1: idle, 2: purifying
  SpanCharacteristic *sc_target_state; // rw, 9-108 0: manual, 1: auto
  SpanCharacteristic *sc_speed; // rw, 9-81 0-100, percentage

  uint8_t _current_state;
  uint8_t _target_state;

 AirPurifier() : Service::AirPurifier() {
    new Characteristic::Name("Solder Fume Air Purifier");

    sc_active = new Characteristic::Active();
    sc_current_state = new Characteristic::CurrentAirPurifierState();
    sc_target_state = new Characteristic::TargetAirPurifierState();
    sc_speed = new Characteristic::RotationSpeed();
  }

  boolean update(){
    if(sc_speed->getVal() != sc_speed->getNewVal()) {
      Serial.print("HOMEKIT set fan speed ");
      Serial.println((uint16_t)sc_speed->getNewVal());
      hk_fan_speed.set((uint16_t)sc_speed->getNewVal());

      //      fan_speed.dump("fan_speed");
      //CHECKIT = true;
    }

    if(sc_active->getVal() != sc_active->getNewVal()) {
      Serial.print("HOMEKIT active ");
      Serial.println(sc_active->getNewVal());
      if(sc_active->getNewVal())
	hk_enabled.set(true);
      else
	hk_enabled.set(false);

      //      enabled.dump("enabled");
      //      CHECKIT = true;
    }

    if(sc_target_state->getVal() != sc_target_state->getNewVal()) {
      Serial.print("HOMEKIT set auto_speed ");
      Serial.println(sc_target_state->getNewVal());
      hk_auto_speed.set(sc_target_state->getNewVal());

      //      auto_speed.dump("auto_speed");
      //      CHECKIT = true;
    }

    if(sc_current_state->getVal() != sc_current_state->getNewVal()) {
      Serial.print("HOMEKIT set current_state ");
      Serial.println(sc_current_state->getNewVal());
    }

    return true;
  }

  void loop() {
    if(fan_speed.dirty) {
      Serial.print("HOMEKIT REPORT fan speed ");
      Serial.println(fan_speed.get());

      sc_speed->setVal(fan_speed.get());

      if(fan_speed.get() != 0)
	sc_current_state->setVal(2);
      else
	sc_current_state->setVal(1);

      fan_speed.clean();
    }

    if(enabled.dirty) {
      Serial.print("HOMEKIT REPORT enabled ");
      Serial.println(enabled.get());

      if(enabled.get())
	sc_active->setVal(true);
      else
	sc_active->setVal(false);

      enabled.clean();
    }
  }
};

struct RgbLED : Service::LightBulb {
  SpanCharacteristic *power;
  SpanCharacteristic *H;
  SpanCharacteristic *S;
  SpanCharacteristic *V;

#define NUM_LEDS 6
  CRGB _leds[NUM_LEDS];

  RgbLED() : Service::LightBulb() {
    power = new Characteristic::On();
    H = new Characteristic::Hue(0);
    S = new Characteristic::Saturation(0);
    V = new Characteristic::Brightness(100);
    V->setRange(5,100,1);
    
    Serial.print("Configuring RGB LED");

    FastLED.addLeds<NEOPIXEL, 14>(_leds, NUM_LEDS);
  }

  boolean update() {
    boolean p;
    float v, h, s, r, g, b;

    h = H->getVal<float>();
    s = S->getVal<float>();
    v = V->getVal<float>();
    p = power->getVal();

    if(power->updated()){
      p=power->getNewVal();
      Serial.printf("Power=%s->%s, ",power->getVal()?"true":"false",p?"true":"false");
    } else {
      Serial.printf("Power=%s, ",p?"true":"false");
    }
      
    if(H->updated()){
      h=H->getNewVal<float>();
      Serial.printf("H=%.0f->%.0f, ",H->getVal<float>(),h);
    } else {
      Serial.printf("H=%.0f, ",h);
    }

    if(S->updated()){
      s=S->getNewVal<float>();
      Serial.printf("S=%.0f->%.0f, ",S->getVal<float>(),s);
    } else {
      Serial.printf("S=%.0f, ",s);
    }

    if(V->updated()){
      v=V->getNewVal<float>();
      Serial.printf("V=%.0f->%.0f  ",V->getVal<float>(),v);
    } else {
      Serial.printf("V=%.0f  ",v);
    }

    // Here we call a static function of LedPin that converts HSV to RGB.
    // Parameters must all be floats in range of H[0,360], S[0,1], and V[0,1]
    // R, G, B, returned [0,1] range as well
    LedPin::HSVtoRGB(h,s/100.0,v/100.0,&r,&g,&b);   // since HomeKit provides S and V in percent, scale down by 100

    int R, G, B;

    R = p*r*100;
    G = p*g*100;
    B = p*b*100;

    Serial.printf("RGB=(%d,%d,%d)\n",R,G,B);

    fill_solid(_leds, NUM_LEDS, CRGB(R, G, B));
      
    return true;
  }
};
