#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

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
date dtreceived;

void setup() {
  Serial.begin(9600);

  radio.begin();
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_2MBPS);
  radio.setChannel(124);
  radio.openReadingPipe(0, address);
  radio.startListening();
}
void loop() {
  if (radio.available()) {
    radio.read(&dtreceived, sizeof(dtreceived));
    Serial.print("Received: ID:");
    Serial.print(dtreceived.id);
    Serial.print(" - ");
    Serial.print(dtreceived.day);
    Serial.print(dtreceived.month);
    Serial.print(dtreceived.year);
    Serial.print(" - ");
    Serial.print(dtreceived.hour);
    Serial.print(":");
    Serial.print(dtreceived.minute);
    Serial.print(":");
    Serial.println(dtreceived.second);
  }
}
