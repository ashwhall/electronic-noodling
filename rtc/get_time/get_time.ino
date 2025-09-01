/*
Based on https://github.com/NorthernWidget/DS3231/blob/master/examples/echo_time/echo_time.ino
*/

#include <DS3231.h>
#include <Wire.h>

DS3231 myRTC;

bool century = false;
bool h12Flag;
bool pmFlag;

void setup() {
	// Start the serial port
	Serial.begin(57600);

	// Start the I2C interface
	Wire.begin();
}

void loop() {
  Serial.print(myRTC.getYear(), DEC);
  Serial.print("-");
  Serial.print(myRTC.getMonth(century), DEC);
  Serial.print("-");
  Serial.print(myRTC.getDate(), DEC);
  Serial.print(" ");
  Serial.print(myRTC.getHour(h12Flag, pmFlag), DEC); //24-hr
  Serial.print(":");
  Serial.print(myRTC.getMinute(), DEC);
  Serial.print(":");
  Serial.println(myRTC.getSecond(), DEC);
  delay(1000);
}