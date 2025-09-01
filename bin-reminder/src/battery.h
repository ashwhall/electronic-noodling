#ifndef BATTERY_H
#define BATTERY_H
#include "pins.h"
#include "util_types.h"

void setup_battery();
void battery_step();
float read_battery_level();

#endif // BATTERY_H
