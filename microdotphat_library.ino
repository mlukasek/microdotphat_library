/*  Micro Dot pHAT library
    Simple Arduino library for PIMORONI Micro Dot pHAT retro LED character display
    Copyright (C) 2020 Martin Lukasek <martin@lukasek.cz>
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>. 

    Connect 5V (pin 2 or 4) of Micro Dot pHAT to 5V pin of M5Stack
    Connect GND (pin 6) of Micro Dot pHAT to GND pin of M5Stack
    Connect SDA (pin 3) of Micro Dot pHAT to SDA pin of M5Stack (GPIO 21)
    Connect SCL (pin 5) of Micro Dot pHAT to SCL pin of M5Stack (GPIO 22)
    
    This software is based on work of  clarktans:
    https://github.com/clarktans/microdot-clock
    TimeNTP_ESP8266WiFi.ino
    Example showing time sync to NTP time source
*/


#include <Arduino.h>
#include <M5Stack.h>
#include <WiFiMulti.h>

#include "microdot.h"

MicroDot MD(0x61);

WiFiMulti WiFiMultiple;
struct tm localTimeInfo;
int MAX_TIME_RETRY = 30;
const int timeZone = 1;   // Central European Time
const int dst = 0;        // Daylight saving time
int lastSec;

void printLocalTime() {
  if(!getLocalTime(&localTimeInfo)){
    Serial.println("Failed to obtain time");
    M5.Lcd.println("Failed to obtain time");
    return;
  }
  Serial.println(&localTimeInfo, "%A, %B %d %Y %H:%M:%S");
  M5.Lcd.println(&localTimeInfo, "%A, %B %d %Y %H:%M:%S");
}

void setup()
{
  M5.begin();
  M5.Lcd.setTextSize(2);
  Serial.begin(115200);
  MD.begin();
  MD.writeString("Hello!");
  delay(300);

  // WiFiMultiple.addAP("SSID", "passpasspass");
  WiFiMultiple.addAP("Skr1474", "dddddddd");

  Serial.println();
  M5.Lcd.println("");
  Serial.print("Wait for WiFi... ");
  M5.Lcd.print("Wait for WiFi... ");

  while(WiFiMultiple.run() != WL_CONNECTED) {
      Serial.print(".");
      M5.Lcd.print(".");
      delay(500);
  }

  Serial.println("");
  M5.Lcd.println("");
  Serial.print("WiFi connected to SSID "); Serial.println(WiFi.SSID());
  M5.Lcd.print("WiFi SSID "); M5.Lcd.println(WiFi.SSID());
  Serial.println("IP address: ");
  M5.Lcd.println("IP address: ");
  Serial.println(WiFi.localIP());
  M5.Lcd.println(WiFi.localIP());

  configTime(timeZone*3600, dst*3600, "pool.ntp.org", "time.nist.gov", "time.google.com");
  delay(1000);
  Serial.print("Waiting for time.");
  int i = 0;
  while(!getLocalTime(&localTimeInfo)) {
    Serial.print(".");
    delay(1000);
    i++;
    if (i > MAX_TIME_RETRY) {
      Serial.print("Gave up waiting for time to have a valid value.");
      break;
    }
  }
  Serial.println();
  printLocalTime();

  lastSec = 61;  // update now
  
  Serial.println("Connection done");
  M5.Lcd.println("Connection done");
}

void loop() {
  char tmpstr[20];

  if(!getLocalTime(&localTimeInfo))
    lastSec = 61;
    
  if(lastSec!=localTimeInfo.tm_sec) {
    lastSec=localTimeInfo.tm_sec;
    sprintf(tmpstr, "%02d%02d%02d", localTimeInfo.tm_hour, localTimeInfo.tm_min, localTimeInfo.tm_sec);
    if(lastSec & 1) {
      tmpstr[2] |= 0x80;  // add decimal dot
      tmpstr[4] |= 0x80;  // add decimal dot
    }
    MD.writeString(tmpstr);
  }

  delay(100);
}
