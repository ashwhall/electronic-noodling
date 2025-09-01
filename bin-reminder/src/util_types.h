#ifndef UTIL_TYPES_H
#define UTIL_TYPES_H
#include <Arduino.h>

typedef void (*Callback)();

enum Mode
{
    /** Device starting up - occurs only once */
    STARTUP,
    /** Device is idle - all lights off, no action to take */
    IDLE,
    /** Displaying the battery level - flashes the active colour a number of times */
    DISPLAYING_BATTERY,
    /** The alarm has been triggered - the relevant light is illuminated */
    ALARM_TRIGGERED,
};

enum Colour
{
    RED,
    YELLOW
};

#endif // UTIL_TYPES_H
