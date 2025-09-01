#ifndef CLOCK_H
#define CLOCK_H
#include <Arduino.h>
#include <RTClib.h>
#include "util_types.h"

void setup_clock(Callback on_alarm1, Callback on_alarm2);
bool set_alarm1();
bool set_alarm2();
void alarm_interrupt();
void clock_step();
String format_date(DateTime date);
String get_current_time_string();

#endif // CLOCK_H
