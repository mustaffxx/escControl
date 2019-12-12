#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

RF24 radio(9, 10); // CE, CSN
const byte address[6] = "node1";

// faixa velocidade 200~300
int received = 0;
int spd = 1000;
int spdAnterior = 1000;

//
Servo esc;

void setup() {
  esc.attach(5);

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
    Serial.print("Velocidade recebida: ");
    Serial.print(received);
    spd = map(received, 0, 100, 1000, 2000);
    Serial.print(" Velocidade ESC: ");
    Serial.println(spd);
    if (spd != spdAnterior) {
      esc.writeMicroseconds(spd);
      Serial.println("Alterou");
    }
    spdAnterior = spd;
  }
}
