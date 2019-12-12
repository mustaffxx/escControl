#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(6, 11); // CE, CSN
const byte address[6] = "node1";

//
int asciRead = 0;
int spd = 0;

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
  asciRead = 0;
  if (Serial.available() > 0) {
    asciRead = Serial.read();
  }
  setSpeed(asciRead);
  sendData();
  Serial.print("Velocidade: ");
  Serial.println(spd);
}

/*
  const char text[] = "Hello World";
  radio.write(&text, sizeof(text));
  delay(1000);
*/


void sendData() {
  radio.write(&spd, sizeof(spd));
  delay(100);
}

void setSpeed(int asciRead) {
  switch (asciRead) {
    case 122:
      spd = 0;
      break;
    case 90:
      spd = 0;
      break;
    case 48:
      spd = 5;
      break;
    case 49:
      spd = 10;
      break;
    case 50:
      spd = 20;
      break;
    case 51:
      spd = 30;
      break;
    case 52:
      spd = 40;
      break;
    case 53:
      spd = 50;
      break;
    case 54:
      spd = 60;
      break;
    case 55:
      spd = 70;
      break;
    case 56:
      spd = 80;
      break;
    case 57:
      spd = 90;
      break;
    case 109:
      spd = 100;
      break;
    case 77:
      spd = 100;
      break;
    case 43:
      spd = spd + 5;
      break;
    case 45:
      spd = spd - 5;
      break;
    default:
      break;
  }
  if (spd < 0) spd = 0;
  if (spd > 100) spd = 100;
}
