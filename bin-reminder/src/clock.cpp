#include <Arduino.h>
#include "clock.h"
#include "util_types.h"
#include "RTClib.h"
#include "pins.h"

Callback on_alarm1_callback = nullptr;
Callback on_alarm2_callback = nullptr;

bool alarm_triggered = false;

bool AUTO_SET_CLOCK = false;

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// 5pm on Thursdays, using DS3231_A1_Day mode
const DateTime ALARM1_TIME = DateTime(2025, 8, 28, 17, 0, 0);
// Match day of week, hours, minutes and seconds
const Ds3231Alarm1Mode ALARM1_MODE = DS3231_A1_Day;
// Midnight Thursday into Friday, using DS3231_A2_Day mode
const DateTime ALARM2_TIME = DateTime(2025, 8, 29, 0, 0, 0);
// Match day of week, hours, minutes and seconds
const Ds3231Alarm2Mode ALARM2_MODE = DS3231_A2_Day;

void setup_clock(Callback on_alarm1, Callback on_alarm2)
{
    on_alarm1_callback = on_alarm1;
    on_alarm2_callback = on_alarm2;

    if (!rtc.begin())
    {
        Serial.println("Couldn't find RTC");
        Serial.flush();
        while (1)
            delay(10);
    }

    if (AUTO_SET_CLOCK)
    {
        Serial.println("Setting RTC time...");
        rtc.adjust(DateTime(DateTime(F(__DATE__), F(__TIME__))));
    }

    pinMode(CLOCK_INTERRUPT_PIN, INPUT_PULLUP);

    // we don't need the 32K Pin, so disable it
    rtc.disable32K();

    Serial.println("System booting at " + get_current_time_string());

    // set alarm 1, 2 flag to false (so alarm 1, 2 didn't happen so far)
    // if not done, this easily leads to problems, as both register aren't reset on reboot/recompile
    rtc.clearAlarm(1);
    rtc.clearAlarm(2);

    // stop oscillating signals at SQW Pin, otherwise setAlarm1 will fail
    rtc.writeSqwPinMode(DS3231_OFF);
    Serial.flush();

    set_alarm1();
    set_alarm2();

    // Attach interrupt last, so the RTC module is set up entirely
    attachInterrupt(digitalPinToInterrupt(CLOCK_INTERRUPT_PIN), alarm_interrupt, FALLING);
}

String get_current_time_string()
{
    return format_date(rtc.now());
}

String format_date(DateTime date)
{
    String yearStr = String(date.year(), DEC);
    String monthStr = (date.month() < 10 ? "0" : "") + String(date.month(), DEC);
    String dayStr = (date.day() < 10 ? "0" : "") + String(date.day(), DEC);
    String hourStr = (date.hour() < 10 ? "0" : "") + String(date.hour(), DEC);
    String minuteStr = (date.minute() < 10 ? "0" : "") + String(date.minute(), DEC);
    String secondStr = (date.second() < 10 ? "0" : "") + String(date.second(), DEC);
    String dayOfWeek = daysOfTheWeek[date.dayOfTheWeek()];

    String formattedTime = dayOfWeek + ", " + yearStr + "-" + monthStr + "-" + dayStr + " " + hourStr + ":" + minuteStr + ":" + secondStr;

    return formattedTime;
}

void alarm_interrupt()
{
    alarm_triggered = true;
}

void clock_step()
{
    if (alarm_triggered)
    {
        alarm_triggered = false;

        if (rtc.alarmFired(1))
        {
            Serial.println("Alarm 1 triggered at " + get_current_time_string());
            Serial.flush();

            rtc.clearAlarm(1);

            if (on_alarm1_callback != nullptr)
            {
                on_alarm1_callback();
            }

            // Re-set alarm 1 for the next occurrence
            set_alarm1();
        }
        if (rtc.alarmFired(2))
        {
            Serial.println("Alarm 2 triggered at " + get_current_time_string());
            Serial.flush();

            rtc.clearAlarm(2);

            if (on_alarm2_callback != nullptr)
            {
                on_alarm2_callback();
            }

            // Re-set alarm 2 for the next occurrence
            set_alarm2();
        }
    }
}

bool set_alarm1()
{
    // TODO: Remove next two lines - trigger every 15 seconds for testing
    DateTime ALARM1_TIME = rtc.now() + TimeSpan(0, 0, 0, 15);
    const Ds3231Alarm1Mode ALARM1_MODE = DS3231_A1_Second;

    Serial.println("Setting alarm 1 for " + format_date(ALARM1_TIME));
    bool result = rtc.setAlarm1(ALARM1_TIME, ALARM1_MODE);
    if (result)
    {
        Serial.println("Alarm 1 set successfully");
    }
    else
    {
        Serial.println("Error setting alarm 1");
    }
    return result;
}

bool set_alarm2()
{
    // TODO: Remove next two lines - trigger on the minute for testing
    DateTime ALARM2_TIME = rtc.now() + TimeSpan(0, 0, 0, 10);
    const Ds3231Alarm2Mode ALARM2_MODE = DS3231_A2_PerMinute;

    Serial.println("Setting alarm 2 for " + format_date(ALARM2_TIME));
    bool result = rtc.setAlarm2(ALARM2_TIME, ALARM2_MODE);
    if (result)
    {
        Serial.println("Alarm 1 set successfully");
    }
    else
    {
        Serial.println("Error setting alarm 1");
    }
    return result;
}
