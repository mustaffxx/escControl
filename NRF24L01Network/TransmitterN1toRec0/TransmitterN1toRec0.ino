#include <SPI.h>
//#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;

RF24 radio(6, 11); // CE, CSN
const byte address[6] = "node1";

//
char sent[32];

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
  String datetime = getDateTime();
  datetime.toCharArray(sent, 32);
   
  radio.write(&sent, sizeof(sent));
  Serial.print("Sent: ");
  Serial.print(sent);
  Serial.print(" Size: ");
  Serial.println(sizeof(sent));
  delay(1000);
}

String getDateTime() {
  DateTime now = rtc.now();
  String id = "ABCDE";
  String days = (String)now.day();
  String months = (String)now.month();
  String years = (String)now.year();
  String hours = (String)now.hour();
  String minutes = (String)now.minute();
  String seconds = (String)now.second();
  String code = id + years + months + days + hours + minutes + seconds;
  return code;
}
