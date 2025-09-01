#include <Arduino.h>
#include "button.h"
#include "pins.h"

Callback on_press_callback = nullptr;
Callback on_hold_callback = nullptr;

unsigned long last_button_press_time = 0;

bool button_pressed = false;

const unsigned long DEBOUNCE_THRESH_MS = 100;
const unsigned long HOLD_THRESH_MS = 500;

// TODO: Minimal sized types

void setup_button(Callback on_press, Callback on_hold)
{
    on_press_callback = on_press;
    on_hold_callback = on_hold;

    pinMode(BUT_PIN, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(BUT_PIN), button_interrupt, FALLING);
}

void button_interrupt()
{
    button_pressed = true;
}

void button_step()
{
    unsigned long curr_time = millis();

    if (button_pressed)
    {
        Serial.print("Press!");
        unsigned long time_since_last = curr_time - last_button_press_time;
        if (time_since_last >= DEBOUNCE_THRESH_MS)
        {
            Serial.print(" (accepted)");
            last_button_press_time = curr_time;

            delay(HOLD_THRESH_MS);
            if (digitalRead(BUT_PIN) == LOW)
            {
                Serial.print(" - hold");
                if (on_hold_callback != nullptr)
                {
                    on_hold_callback();
                }
                Serial.println();
                Serial.flush();
            }
            else
            {
                if (on_press_callback != nullptr)
                {
                    on_press_callback();
                }
            }
        }
        else
        {
            Serial.print(" (ignored)");
        }
        Serial.println();
        Serial.flush();
        button_pressed = false;
    }
}
