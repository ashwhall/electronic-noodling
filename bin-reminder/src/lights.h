#ifndef LIGHTS_H
#define LIGHTS_H
#include <Arduino.h>
#include "util_types.h"

void setup_lights();
void turn_off_all_lights();
void turn_on(Colour light);
void turn_off(Colour light);
void flash(Colour light, uint8_t times);
void flash(Colour light, uint8_t times, unsigned long delay_ms);
void flash_active(uint8_t times);
void flash_active(uint8_t times, unsigned long delay_ms);

void begin_startup_lights();
void end_startup_lights();

void announce_colour_change();

void turn_on_alarm_lights();

#endif // LIGHTS_H
