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

void setup() {
  Serial.begin(9600);

  radio.begin();
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_1MBPS);
  radio.setChannel(119);

  //radio.openWritingPipe(address[0]);
  radio.openReadingPipe(1, address[1]);
  radio.startListening();
}

void loop() {
  delay(5);
  if (radio.available()) {
    //Serial.println("Stop 1");
    //memset(msg, 0, packageLen); // clear array before store

    radio.read(&msg, packageLen);

    /*
      memset(msgAux, 0, packageLen);
      for (int i = 0; i < packageLen; i++) {
      msgAux[i] = msg[i];
      Serial.print(msg[i]);
      }
    */
    Serial.print(msg);
    Serial.print(" Size: ");
    Serial.println(strlen(msg));

    String toComp = "";
    toComp = String(msg[0]) + String(msg[1]) + String(msg[4]) + String(msg[5]);
    //Serial.println(toComp);

    if (toComp == "tttt") {
      Serial.println("I'm here");
      // to check if received all packages
      int howmanyPackages = (String(msg[2]) + String(msg[3])).toInt();
      int howmanyPackagesReceived[howmanyPackages];
      unsigned int sumNumberPackagesNeeded = 0;

      char msgArray[howmanyPackages][14];

      for (int i = 0; i < howmanyPackages; i++) {
        howmanyPackagesReceived[i] = 0; // populate array
        sumNumberPackagesNeeded += i;
      }

      //Serial.print("Number of packages: ");
      //Serial.println(howmanyPackages);
      while (1) {
        if (radio.available()) {
          unsigned int sumNumberPackagesReceived = 0;

          radio.read(&msg, sizeof(msg));

          if ((msg[1] == 'i') || (msg[1] == 'f')) {

            for (int i = 0; i < packageLen - abs(packageLen - int(strlen(msg))); i++) {
              msgArray[(String(msg[0])).toInt()][i] = msg[i + 2];
              Serial.print(msg[i]);
            }
            Serial.print(" pos: ");
            Serial.print(packageLen - abs(packageLen - int(strlen(msg))));
            Serial.println();
            Serial.print("Received packages: ");
            Serial.print(sumNumberPackagesReceived);
            Serial.print(" Packages needed: ");
            Serial.println(sumNumberPackagesNeeded);

            howmanyPackagesReceived[(String(msg[0])).toInt()] = (String(msg[0])).toInt();
            Serial.print("howmanyPackagesReceived: ");
            //Serial.println(howmanyPackagesReceived[(String(msg[0])).toInt()]);

            for (int i = 0; i < howmanyPackages; i++) {
              sumNumberPackagesReceived += howmanyPackagesReceived[i];
              Serial.print(howmanyPackagesReceived[i]);
              Serial.print(", ");
            }
            Serial.println();
            Serial.println();

            if (sumNumberPackagesReceived == sumNumberPackagesNeeded) {
              Serial.print("Received packages: ");
              Serial.print(sumNumberPackagesReceived);
              Serial.print(" Packages needed: ");
              Serial.println(sumNumberPackagesNeeded);
              break;
            }


          }
        }
      }

      Serial.println("##########");
      for (int i = 0; i < howmanyPackages; i++) {
        for (int j = 0; j < packageLen; j++) {
          Serial.print(msgArray[i][j]);
          msgArray[i][j] = '0';
        }
        Serial.println();
      }
    }
  }
}
