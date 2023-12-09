#pragma once

#include "dirty.h"

void solderfume_setup();
void solderfume_loop();
void solderfume_stream();

extern Dirty<float> pm10_value;
extern Dirty<float> pm25_value;
extern Dirty<uint16_t> fan_speed;
extern Dirty<bool> enabled;
extern Dirty<bool> auto_speed;

extern Dirty<uint16_t> hk_fan_speed;
extern Dirty<bool> hk_enabled;
extern Dirty<bool> hk_auto_speed;


extern boolean CHECKIT;

