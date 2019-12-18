#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(6, 11); // CE, CSN
const byte address[6] = "node1";

//
String readString = "";
char *msg = "Hello World";

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
  while (Serial.available()) {
    char c = Serial.read();
    readString += c;
    delay(2);
  }
  /*
  if (readString.length() > 0) {
    //sendData(readString);
    //int num = 10;
    //radio.write(&num, sizeof(num));
    Serial.print("Enviado: ");
    Serial.println(readString);
    readString = "";
  }
  */
  Serial.print("Enviado: ");
  Serial.println(msg);
  radio.write(&msg, strlen(msg));
}

void sendData(String data) {
  radio.write(&data, sizeof(data));
  delay(100);
}
