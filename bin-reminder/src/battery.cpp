#include "battery.h"
#include <Arduino.h>
#include "lights.h"
#include "state.h"

// const float R1 = 680000.0; // Ohms
// const float R2 = 470000.0; // Ohms
const float R1 = 704000.0; // Ohms (measured)
const float R2 = 472000.0; // Ohms (measured)

// Approximate voltage-to-percentage table (Li-ion single cell)
const float voltage_lookup[] = {4.20, 4.10, 3.95, 3.85, 3.75, 3.50, 3.30};
const float percentage_lookup[] = {1, 0.9, 0.75, 0.5, 0.25, 0.1, 0};

void setup_battery()
{
    pinMode(BAT_READ_PIN, INPUT);
    pinMode(BAT_CHECK_PIN, INPUT);
}

float voltage_to_percent(float v)
{
    if (v >= voltage_lookup[0])
        return 1;
    if (v <= voltage_lookup[6])
        return 0;

    for (int i = 0; i < 6; i++)
    {
        if (v <= voltage_lookup[i] && v > voltage_lookup[i + 1])
        {
            // Linear interpolation
            float slope = (percentage_lookup[i + 1] - percentage_lookup[i]) / (voltage_lookup[i + 1] - voltage_lookup[i]);
            return (float)percentage_lookup[i] + slope * (v - voltage_lookup[i]);
        }
    }
    return 0;
}

float read_battery_level()
{
    pinMode(BAT_CHECK_PIN, OUTPUT);
    digitalWrite(BAT_CHECK_PIN, HIGH);
    delay(10); // Let the pin stabilise
    // Read the battery level

    // TODO: Actual battery level reading
    int raw = analogRead(BAT_READ_PIN);

    pinMode(BAT_CHECK_PIN, INPUT);
    // Convert ADC reading back to battery voltage
    float vRef = 3.3; // ADC reference (Vcc)
    float vSense = (raw / 1023.0) * vRef;
    float vBatt = vSense * (R1 + R2) / R2;

    float pct = (float)voltage_to_percent(vBatt) / 100.0f;
    return pct;
}

/**
 * Convert a percentage (0.0 to 1.0) to a decile (1 to 10),
 * where 1 means 0-10%, 2 means 11-20%, ..., 10 means 91-100%
 */
int to_decile(float pct)
{
    int out = (int)(ceil(pct * 10.0f));
    return clamp(out, 1, 10);
}

void display_battery_level()
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
    turn_off_all_lights();
    delay(200);
    flash_active(decile_level);
}
