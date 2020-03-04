// arduino micro use 6 CE, 11 CSN
// arduino uno use 9 CE, 10 CSN
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10); // CE, CSN
const byte address[][6] = {"node1", "node2"};

//
const int packageLen = 16;
char msg[packageLen] = "";
char msgAux[packageLen] = "";
//
String msgArray[3][14];

void setup() {
  Serial.begin(9600);

  radio.begin();
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_1MBPS);
  radio.setChannel(119);

  //radio.openWritingPipe(address[0]);
  radio.openReadingPipe(1, address[1]);
  delay(500);

}
void loop() {
  delay(5);
  radio.startListening();

  int index = 0;
  if (radio.available()) {
    //Serial.println("Stop 1");
    radio.read(&msg, sizeof(msg));
    for (int i = 0; i < packageLen; i++) {
      msgAux[i] = msg[i];
    }
    msgAux[packageLen] = '\0';
    Serial.print("Recebido: ");
    Serial.print(msgAux);
    Serial.print(" Size: ");
    Serial.println(sizeof(msgAux));
    Serial.println();

    String toComp = "";
    toComp = String(msgAux[0]) + String(msgAux[1]) + String(msgAux[4]) + String(msgAux[5]);
    Serial.println(toComp);

    if (toComp == "tttt") {
      int howmanyPackages = (String(msgAux[2]) + String(msgAux[3])).toInt();
      int i = 0;
      while (1) {
        radio.read(&msg, sizeof(msg));
        if (msg[1] == 'i') {

          // in working...
        }
      }
    }
  }
  msg[0] = '\0';
  msgAux[0] = '\0';
  //index = 0;
  //Serial.println();
}
