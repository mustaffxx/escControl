#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10); // CE, CSN
const byte address2[6] = "node2";

//
float rpm = 0.0;

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_2MBPS);
  radio.setChannel(125);
  radio.openReadingPipe(0, address2);
  radio.startListening();
}
void loop() {

  if (radio.available()) {
    radio.read(&rpm, sizeof(rpm));
    Serial.print("RPM: ");
    Serial.println(rpm);
  }
  delay(100);
}
