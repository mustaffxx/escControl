#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;

RF24 radio(6, 11); // CE, CSN
const byte address[6] = "node1";

//
struct date {
  uint8_t id;
  uint8_t day;
  uint8_t month;
  uint16_t year;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
};
date dtsend;

void setup() {
  Serial.begin(9600);
  if (!rtc.begin()) {
    Serial.println("DS3231 not found");
    while (1);
  }

  radio.begin();
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_2MBPS);
  radio.setChannel(124);
  radio.openWritingPipe(address);
  radio.stopListening();
}

void loop() {
  DateTime now = rtc.now();
  dtsend.id = 0001;
  dtsend.day = (uint8_t)now.day();
  dtsend.month = (uint8_t)now.month();
  dtsend.year = (uint16_t)now.year();
  dtsend.hour = (uint8_t)now.hour();
  dtsend.minute = (uint8_t)now.minute();
  dtsend.second = (uint8_t)now.second();

  radio.write(&dtsend, sizeof(dtsend));
  Serial.print("Sent: ID:");
  Serial.print(dtsend.id);
  Serial.print(" - ");
  Serial.print(dtsend.day);
  Serial.print(dtsend.month);
  Serial.print(dtsend.year);
  Serial.print(" - ");
  Serial.print(dtsend.hour);
  Serial.print(":");
  Serial.print(dtsend.minute);
  Serial.print(":");
  Serial.println(dtsend.second);
  delay(1000);
}
