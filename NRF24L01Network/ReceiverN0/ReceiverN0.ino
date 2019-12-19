#include <SPI.h>
//#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(6, 11); // CE, CSN
const byte address[6] = "node1";

//
char received[32];

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
    radio.read(&received, sizeof(received));
    Serial.print("Received: ID: ");
    Serial.print(received);
    Serial.print(" Size: ");
  Serial.println(sizeof(received));
  }
}
