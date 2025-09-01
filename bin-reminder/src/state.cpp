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
