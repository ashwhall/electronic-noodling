#include <Arduino.h>
#include <LowPower.h>
#include "util_types.h"
#include "RTClib.h"
#include "lights.h"
#include "pins.h"
#include "button.h"
#include "clock.h"
#include "battery.h"
#include "state.h"

// TODO: Add my own resister instead of internal pullup
// TODO: Remove built-in LED

void on_button_press()
{
  next_mode();
}

void on_button_hold()
{
  toggle_active_colour();
  announce_colour_change();
}

/** Alarm 1 switches into "triggered" mode */
void on_alarm1()
{
  mode = Mode::ALARM_TRIGGERED;
}

/** Alarm 2 sets toggles the active colour and returns to idle mode */
void on_alarm2()
{
  mode = Mode::IDLE;

  toggle_active_colour();
  announce_colour_change();
}

void begin_startup_lights_seq()
{
  flash(Colour::RED, 10, 25);
}

void finish_startup_lights_seq()
{
  flash(Colour::YELLOW, 10, 25);
}

void setup()
{
  Serial.begin(9600);
  while (!Serial)
  {
  };

  setup_lights();

  begin_startup_lights_seq();

  setup_button(&on_button_press, &on_button_hold);
  setup_battery();
  setup_clock(&on_alarm1, &on_alarm2);

  // Let everything stabilise
  delay(500);
}

void loop()
{
  button_step();

  if (mode == Mode::DISPLAYING_BATTERY)
  {
    Serial.println("DISPLAYING_BATTERY");
  }
  if (mode == Mode::ALARM_TRIGGERED)
  {
    Serial.println("ALARM_TRIGGERED");
  }
  if (mode == Mode::IDLE)
  {
    Serial.println("IDLE");
  }
  Serial.flush();

  if (mode == Mode::STARTUP)
  {
    finish_startup_lights_seq();
    mode = Mode::DISPLAYING_BATTERY;
  }

  clock_step();
  lights_step();
  battery_step();

  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
}
