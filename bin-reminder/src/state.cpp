#include "state.h"

Mode mode = Mode::STARTUP;
Colour active_colour = Colour::RED;

Mode next_mode()
{
    switch (mode)
    {
    case Mode::STARTUP:
        mode = Mode::DISPLAYING_BATTERY;
        break;
    case Mode::IDLE:
        mode = Mode::ALARM_TRIGGERED;
        break;
    case Mode::ALARM_TRIGGERED:
        mode = Mode::DISPLAYING_BATTERY;
        break;
    case Mode::DISPLAYING_BATTERY:
        mode = Mode::IDLE;
        break;
    default:
        mode = Mode::IDLE;
        break;
    }

    return mode;
}

Colour toggle_active_colour()
{
    if (active_colour == Colour::YELLOW)
    {
        active_colour = Colour::RED;
    }
    else
    {
        active_colour = Colour::YELLOW;
    }

    return active_colour;
}

void print_state()
{
#ifdef DEBUG
    Serial.print("Mode: ");
    switch (mode)
    {
    case Mode::STARTUP:
        Serial.print("STARTUP");
        break;
    case Mode::IDLE:
        Serial.print("IDLE");
        break;
    case Mode::DISPLAYING_BATTERY:
        Serial.print("DISPLAYING_BATTERY");
        break;
    case Mode::ALARM_TRIGGERED:
        Serial.print("ALARM_TRIGGERED");
        break;
    default:
        Serial.print("UNKNOWN");
        break;
    }
    Serial.print(", Active Colour: ");
    switch (active_colour)
    {
    case Colour::RED:
        Serial.println("RED");
        break;
    case Colour::YELLOW:
        Serial.println("YELLOW");
        break;
    default:
        Serial.println("UNKNOWN");
        break;
    }
    Serial.flush();
#endif
}
