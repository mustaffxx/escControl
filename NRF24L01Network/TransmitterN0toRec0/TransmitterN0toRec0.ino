#include <SPI.h>
//#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(6, 11); // CE, CSN
const byte address[6] = "node1";

//
char sent[32];

void setup() {
  Serial.begin(9600);

  radio.begin();
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_2MBPS);
  radio.setChannel(124);
  radio.openWritingPipe(address);
  radio.stopListening();
}

void loop() {
  String stamp = "EDCBA00000000000000";
  stamp.toCharArray(sent, 32);

  radio.write(&sent, sizeof(sent));
  Serial.print("Sent: ");
  Serial.print(sent);
  Serial.print(" Size: ");
  Serial.println(sizeof(sent));
  delay(1000);
}
