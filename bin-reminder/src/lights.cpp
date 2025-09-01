#include <Arduino.h>
#include "lights.h"
#include "pins.h"
#include "util_types.h"
#include "state.h"

const int FLASH_DELAY_MS = 200;

uint8_t pin_from_colour(Colour light)
{
    if (light == Colour::YELLOW)
    {
        return YEL_PIN;
    }
    else
    {
        return RED_PIN;
    }
}

void setup_lights()
{
    pinMode(pin_from_colour(Colour::RED), OUTPUT);
    pinMode(pin_from_colour(Colour::YELLOW), OUTPUT);

    all_off();
}

void all_off()
{
    turn_off(Colour::RED);
    turn_off(Colour::YELLOW);
}

void turn_on(Colour light)
{
    digitalWrite(pin_from_colour(light), HIGH);
}

void turn_off(Colour light)
{
    digitalWrite(pin_from_colour(light), LOW);
}

void flash(Colour light, uint8_t times)
{
    flash(light, times, FLASH_DELAY_MS);
}

void flash(Colour light, uint8_t times, unsigned long delay_ms)
{
    for (uint8_t i = 0; i < times; i++)
    {
        turn_on(light);
        delay(delay_ms);
        turn_off(light);
        delay(delay_ms);
    }
}

void flash_active(uint8_t times)
{
    flash(active_colour, times);
}

void flash_active(uint8_t times, unsigned long delay_ms)
{
    flash(active_colour, times, delay_ms);
}

void lights_step()
{
    if (mode == Mode::ALARM_TRIGGERED)
    {
        all_off();
        turn_on(active_colour);
    }
    else
    {
        all_off();
    }
}

void announce_colour_change()
{
    // Flash the new colour a few times to indicate the change
    all_off();
    flash_active(1, 250);

    flash_active(5, 50);
}
