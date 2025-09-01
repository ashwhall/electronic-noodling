#ifndef BUTTON_H
#define BUTTON_H
#include <Arduino.h>
#include "util_types.h"

void setup_button(Callback on_press, Callback on_hold);
void button_interrupt();
void button_step();

#endif // BUTTON_H
