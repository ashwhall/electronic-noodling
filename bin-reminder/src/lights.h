#ifndef LIGHTS_H
#define LIGHTS_H
#include <Arduino.h>
#include "util_types.h"

void setup_lights();
uint8_t pin_from_colour(Colour light);
void all_off();
void turn_on(Colour light);
void turn_off(Colour light);
void flash(Colour light, uint8_t times);
void flash(Colour light, uint8_t times, unsigned long delay_ms);
void flash_active(uint8_t times);
void flash_active(uint8_t times, unsigned long delay_ms);

void announce_colour_change();

void lights_step();

#endif // LIGHTS_H
