#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10); // CE, CSN
const byte address[6] = "node1";

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
    char text[32] = "NOTHING";
    radio.read(&text, sizeof(text));
    Serial.println(text);
  }
}
