/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-date-time-ntp-client-server-arduino/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/
#define uS_TO_S_FACTOR 1000000  //Conversion factor for micro seconds to seconds
        //Time ESP32 will go to sleep (in seconds)
#include <WiFi.h>
#include "time.h"
#include "Arduino.h"
#include "heltec.h"
const char* ssid     = "...";
const char* password = "...";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

void setup(){
  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Disable*/, false /*Serial Enable*/);
  Heltec.display->flipScreenVertically();
  Heltec.display->clear();
  Serial.begin(115200);
  print_wakeup_reason();
  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  
  
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  //Set timer
  time_t now;
  time(&now);
  time_t v=now;
  while((v-now)<=0)
  {
    
    v=inputTime();
   
    time(&now);
    Serial.println("Now is" + String(now));
    Serial.println("To wake at " + String(v));
     Serial.println("Wake up in "+String(v-now)+" seconds");
  }
  
  esp_sleep_enable_timer_wakeup((v-now) * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(v-now) +
  " Seconds");
  //Go to sleep now
  esp_deep_sleep_start();
}

void loop(){
  
  
  
}
void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  switch(wakeup_reason)
  {
    case 1  : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case 2  : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case 3  : Serial.println("Wakeup caused by touchpad"); break;
    case 4  : Serial.println("Wakeup caused by timer"); break;
    case 5  : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.println("Wakeup was not caused by deep sleep"); break;
  }
}
time_t inputTime()
{
  Serial.println("Kada da se probudim(H:M:S)");
  while (Serial.available() == 0) {printLocalTime(); delay(1000);}//wait for data available
  String s1 = Serial.readString();
  char vreme[s1.length()+1];
  Serial.println(s1);
  s1.toCharArray(vreme, s1.length()+1);
  Serial.println(vreme);
  struct tm tm1;
  if(!getLocalTime(&tm1)){
    Serial.println("Failed to obtain time");
    Heltec.display->clear();
    Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
    Heltec.display->setFont(ArialMT_Plain_10);
    Heltec.display->drawString(0, 0, "Failed to obtain time");
    Heltec.display->display();
  }
  strptime(vreme, "%H:%M:%S", &tm1);
  time_t t = mktime(&tm1);
  Serial.println(t);
  return t; 
  
}
void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    Heltec.display->clear();
    Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
    Heltec.display->setFont(ArialMT_Plain_10);
    Heltec.display->drawString(0, 0, "Failed to obtain time");
    Heltec.display->display();
    return;
  }
  
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  char datum[32];
  strftime(datum,32, "%A, %B %d %Y      ", &timeinfo);
  char vreme[32];
  strftime(vreme,32, "%H:%M:%S",&timeinfo);
  
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0, 0, String(datum)+"     ");
  Heltec.display->setFont(ArialMT_Plain_16);
    Heltec.display->drawString(0, 10, String(vreme));
  Heltec.display->display();
//  Serial.print("Day of week: ");
//  Serial.println(&timeinfo, "%A");
//  Serial.print("Month: ");
//  Serial.println(&timeinfo, "%B");
//  Serial.print("Day of Month: ");
//  Serial.println(&timeinfo, "%d");
//  Serial.print("Year: ");
//  Serial.println(&timeinfo, "%Y");
//  Serial.print("Hour: ");
//  Serial.println(&timeinfo, "%H");
//  Serial.print("Hour (12 hour format): ");
//  Serial.println(&timeinfo, "%I");
//  Serial.print("Minute: ");
//  Serial.println(&timeinfo, "%M");
//  Serial.print("Second: ");
//  Serial.println(&timeinfo, "%S");
//
//  Serial.println("Time variables");
//  char timeHour[3];
//  strftime(timeHour,3, "%H", &timeinfo);
//  Serial.println(timeHour);
//  char timeWeekDay[10];
//  strftime(timeWeekDay,10, "%A", &timeinfo);
//  Serial.println(timeWeekDay);
  time_t now;
  time(&now);
  Serial.print("UNIX TS: ");
  Serial.println(now);
  
  Serial.println();
}
