#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

//
RF24 radio(6, 11); // CE, CSN
const byte address[6] = "node1";

//
int received = 0;
int spd = 1000;
int spdAnterior = 1000;

//
Servo esc;

//
int inverter = 0;
unsigned long currentMillis = 0;
unsigned long previousMillis = 0; // will store last time LED was updated
long interval = 0;
bool ledState = LOW;

void setup() {
  esc.attach(5);
  pinMode(3, OUTPUT);
  delay(300);
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

    currentMillis = millis();

    inverter = map(received, 0, 100, 100, 0);
    interval = inverter * 10;

    if (currentMillis - previousMillis >= interval) {
      // save the last time you blinked the LED
      previousMillis = currentMillis;

      // if the LED is off turn it on and vice-versa:
      if (received != 0) {
        if (ledState == LOW) {
          ledState = HIGH;
        } else {
          ledState = LOW;
        }
      } else {
        ledState = LOW;
      }

      // set the LED with the ledState of the variable:
      digitalWrite(3, ledState);
    }

    spd = map(received, 0, 100, 1000, 2000);
    if (spd != spdAnterior) {
      esc.writeMicroseconds(spd);
    }
    spdAnterior = spd;
  }
}
