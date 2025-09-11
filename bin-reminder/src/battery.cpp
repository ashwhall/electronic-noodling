#include "battery.h"
#include <Arduino.h>
#include "lights.h"
#include "state.h"

// const float R1 = 680000.0; // Ohms
// const float R2 = 470000.0; // Ohms
const float R1 = 704000.0; // Ohms (measured)
const float R2 = 472000.0; // Ohms (measured)

const float DIVIDER = (R1 + R2) / R2;

// Approximate voltage-to-percentage table (Li-ion single cell)
// https://intofpv.com/t-lipo-voltage-quick-chart
const uint8_t V_LOOKUP_SIZE = 21;
const float voltage_lookup[V_LOOKUP_SIZE][2] = {
    {1.00, 4.20},
    {0.95, 4.15},
    {0.90, 4.11},
    {0.85, 4.08},
    {0.80, 4.02},
    {0.75, 3.98},
    {0.70, 3.95},
    {0.65, 3.91},
    {0.60, 3.87},
    {0.55, 3.85},
    {0.50, 3.84},
    {0.45, 3.82},
    {0.40, 3.80},
    {0.35, 3.79},
    {0.30, 3.77},
    {0.25, 3.75},
    {0.20, 3.73},
    {0.15, 3.71},
    {0.10, 3.69},
    {0.05, 3.61},
    {0.00, 3.27}};

void setup_battery()
{
    pinMode(BAT_READ_PIN, INPUT);
}

float voltage_to_percent(float v)
{
    if (v >= voltage_lookup[0][1])
        return 1;
    if (v <= voltage_lookup[V_LOOKUP_SIZE - 1][1])
        return 0;

    for (int i = 0; i < V_LOOKUP_SIZE; i++)
    {
        float p1 = voltage_lookup[i][0];
        float p2 = voltage_lookup[i + 1][0];
        float v1 = voltage_lookup[i][1];
        float v2 = voltage_lookup[i + 1][1];
        if (v <= v1 && v > v2)
        {
#if defined(DEBUG)
            Serial.print("v: ");
            Serial.print(v);
            Serial.print(" between ");
            Serial.print(v1);
            Serial.print(" and ");
            Serial.print(v2);
            Serial.print(" => ");
            Serial.print(p1);
            Serial.print(" and ");
            Serial.println(p2);
            Serial.flush();
#endif
            // Linear interpolation
            float slope = (p2 - p1) / (v2 - v1);
            return (float)p1 + slope * (v - v1);
        }
    }
    return 0;
}

float read_vcc()
{
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
    delay(2);
    ADCSRA |= _BV(ADSC);
    while (bit_is_set(ADCSRA, ADSC))
        ;
    float result = 1125300L / ADC; // Back-calculate VCC in mV
    return result / 1000.0 + 0.05; // Correction factor
}

float read_battery_level()
{
    // Read the battery level
    int raw = analogRead(BAT_READ_PIN);

#if defined(DEBUG)
    float vcc = read_vcc();
    Serial.print("Vcc: ");
    Serial.println(vcc);
    Serial.flush();
#endif

#if defined(DEBUG)
    Serial.print("Raw battery reading: ");
    Serial.println(raw);
    Serial.flush();
#endif

    // Convert ADC reading back to battery voltage
    float vRef = vcc; // ADC reference (Vcc)
    float vSense = (raw / 1024.0) * vRef;

#if defined(DEBUG)
    Serial.print("vSense: ");
    Serial.println(vSense);
    Serial.flush();
#endif

    float vBatt = vSense * DIVIDER;

#if defined(DEBUG)
    Serial.print("vBatt: ");
    Serial.println(vBatt);
    Serial.flush();
#endif

    float pct = (float)voltage_to_percent(vBatt);
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

#if defined(DEBUG)
    Serial.print("Battery level: ");
    Serial.print((float)level);
    Serial.flush();
#endif

    turn_off_all_lights();
    delay(200);
    flash_active(decile_level);
}
