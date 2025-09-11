#ifndef CLOCK_H
#define CLOCK_H
#include <Arduino.h>
#include <RTClib.h>
#include "util_types.h"

void setup_clock(Callback on_alarm1, Callback on_alarm2);
bool set_alarm1();
bool set_alarm2();
void alarm_interrupt();
void check_clock_interrupts();
void display_time_by_flashing();
String format_date(DateTime date);
String get_current_time_string();

#endif // CLOCK_H
