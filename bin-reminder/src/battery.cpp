#include "battery.h"
#include <Arduino.h>
#include "lights.h"
#include "state.h"

void setup_battery() {}

float read_battery_level()
{
    // TODO: Actual battery level reading
    int value = analogRead(BAT_PIN);
    return (float)value / 1023.0f;
}

/**
 * Convert a percentage (0.0 to 1.0) to a decile (1 to 10).
 * Ceils the value so that any non-zero percentage is at least 1.
 * Clamps to 10 if over 100%.
 */
int to_decile(float pct)
{
    int out = (int)(ceil(pct * 10.0f));
    if (out < 1)
    {
        return 1;
    }
    else if (out > 10)
    {
        return 10;
    }
    return out;
}

void battery_step()
{
    if (mode == Mode::DISPLAYING_BATTERY)
    {
        float level = read_battery_level();
        int decile_level = to_decile(level);
        Serial.print("Battery level: ");
        Serial.print((float)level);
        Serial.flush();

        if (decile_level < 1)
        {
            decile_level = 1;
        }
        else if (decile_level > 10)
        {
            decile_level = 10;
        }
        all_off();
        delay(200);
        flash_active(decile_level);
        next_mode();
    }
}
