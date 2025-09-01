#include <LowPower.h>
#include "RTClib.h"


RTC_DS3231 rtc;
const int CLOCK_INTERRUPT_PIN = 2;  // Connect to DS3231 SQW pin
const int RED_PIN = 7;
const int YEL_PIN = 6;
const int BUT_PIN = 3;
unsigned long lastButtonInterruptTime = 0;
bool alarmTriggered = false;
bool buttonTriggered = false;

int yelState = LOW;
int redState = HIGH;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

    // Trigger an interrupt when the alarm happens
  pinMode(CLOCK_INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(CLOCK_INTERRUPT_PIN), onAlarm, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUT_PIN), onButton, FALLING);

  pinMode(RED_PIN, OUTPUT);
  pinMode(YEL_PIN, OUTPUT);
  digitalWrite(RED_PIN, redState);
  digitalWrite(YEL_PIN, yelState);
  pinMode(BUT_PIN, INPUT_PULLUP);

  //we don't need the 32K Pin, so disable it
  rtc.disable32K();

  Serial.println("Setting time...");
  rtc.adjust(DateTime(DateTime(F(__DATE__), F(__TIME__))));

  DateTime now = rtc.now();

  // Getting each time field in individual variables
  // And adding a leading zero when needed;
  String yearStr = String(now.year(), DEC);
  String monthStr = (now.month() < 10 ? "0" : "") + String(now.month(), DEC);
  String dayStr = (now.day() < 10 ? "0" : "") + String(now.day(), DEC);
  String hourStr = (now.hour() < 10 ? "0" : "") + String(now.hour(), DEC); 
  String minuteStr = (now.minute() < 10 ? "0" : "") + String(now.minute(), DEC);
  String secondStr = (now.second() < 10 ? "0" : "") + String(now.second(), DEC);
  String dayOfWeek = daysOfTheWeek[now.dayOfTheWeek()];

  // Complete time string
  String formattedTime = dayOfWeek + ", " + yearStr + "-" + monthStr + "-" + dayStr + " " + hourStr + ":" + minuteStr + ":" + secondStr;

  // Print the complete formatted time
  Serial.println("System booting at " + formattedTime);
  // Getting temperature
  // Serial.print(rtc.getTemperature());

  // set alarm 1, 2 flag to false (so alarm 1, 2 didn't happen so far)
  // if not done, this easily leads to problems, as both register aren't reset on reboot/recompile
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);

  // stop oscillating signals at SQW Pin, otherwise setAlarm1 will fail
  rtc.writeSqwPinMode(DS3231_OFF);

  // turn off alarm 2 (in case it isn't off already)
  // again, this isn't done at reboot, so a previously set alarm could easily go overlooked
  rtc.disableAlarm(2);

  if(!rtc.setAlarm1(DateTime(2025, 8, 23, 13, 38, 5), DS3231_A1_Day)) {  // this mode triggers the alarm when the minutes match
    Serial.println("Error, alarm wasn't set!");
  }else {
    Serial.println("Alarm 1 will happen at specified time");
  }

  delay(3000);
}

void onButton() {
  buttonTriggered = true;
}

void onAlarm() {
  alarmTriggered = true;
}

void loop() {
  // put your main code here, to run repeatedly:
  if (alarmTriggered) {
    Serial.println("Alarm triggered!");
    alarmTriggered = false;
  }
  if (buttonTriggered) {
    unsigned long interruptTime = millis();
    if (interruptTime - lastButtonInterruptTime < 200) {
      return;
    }
    lastButtonInterruptTime = interruptTime;
    Serial.println("Button pressed!");

    yelState = yelState == HIGH ? LOW : HIGH;
    redState = redState == HIGH ? LOW : HIGH;
    
    digitalWrite(RED_PIN, redState);
    digitalWrite(YEL_PIN, yelState);

    buttonTriggered = false;
  }
  
  Serial.println("Sleeping for a while...");
  Serial.flush();
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  Serial.println("Awake!");
  Serial.flush();
}
