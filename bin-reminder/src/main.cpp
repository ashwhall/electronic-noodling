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

// TODO: Add my own resister instead of internal pullup - 100kΩ to 470kΩ

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

void setup()
{
#ifdef DEBUG
  Serial.begin(9600);
  while (!Serial)
    ;
#endif

  setup_lights();

  delay(100);

  begin_startup_lights();

  setup_button(&on_button_press, &on_button_hold);
  setup_battery();
  setup_clock(&on_alarm1, &on_alarm2);

  // Let everything stabilise
  delay(500);
}

void loop()
{
  check_button_interrupts();
  check_clock_interrupts();

  bool repeat_loop = false;
  do
  {
    repeat_loop = false;

    print_state();

    switch (mode)
    {
    case Mode::STARTUP:
      end_startup_lights();
      delay(500);
      display_time_by_flashing();

      mode = Mode::DISPLAYING_BATTERY;
      repeat_loop = true;
      break;
    case Mode::DISPLAYING_BATTERY:
      display_battery_level();
      mode = Mode::IDLE;
      break;
    case Mode::ALARM_TRIGGERED:
      turn_on_alarm_lights();
      break;
    case Mode::IDLE:
      turn_off_all_lights();
      break;
    default:
      mode = Mode::IDLE;
      repeat_loop = true;
      break;
    }
  } while (repeat_loop);

  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
}
