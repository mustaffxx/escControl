// this code implement a algorithm to send data greater than 32kbps with nrf24l01+pa+lna
// only works with 126 max size array
// lost a (maybe) relevant number of packages
// todo: increase max size

// arduino micro use 6 CE, 11 CSN
// arduino uno use 9 CE, 10 CSN
#include <SPI.h>
//#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10); // CE, CSN
const byte address[][6] = {"node1", "node2"};

// long message
char charMyContent[126];
unsigned int charSize = 0;

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
  readLongMessage(radio, charMyContent, &charSize); //
  Serial.print("Received: ");
  Serial.println(charMyContent);
}

char* readLongMessage(RF24 radio, char charMyContent[], int *charSize) {
  unsigned int msgLen = 0;
  const int packageLen = 16;
  char msg[packageLen] = "";
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
    //    Serial.print(msg);
    //    Serial.print(" Size: ");
    //    Serial.println(strlen(msg));

    String toComp = "";
    toComp = String(msg[0]) + String(msg[1]) + String(msg[4]) + String(msg[5]);
    //Serial.println(toComp);

    if (toComp == "tttt") {
      //      Serial.println("I'm here");
      // to check if received all packages
      int howmanyPackages = (String(msg[2]) + String(msg[3])).toInt();
      //      Serial.println();
      //      Serial.println();
      //      Serial.print(howmanyPackages);
      //      Serial.println();
      //      Serial.println();
      int howmanyPackagesReceived[howmanyPackages];
      unsigned int sumNumberPackagesNeeded = 0;
      unsigned int lastLenMSG[howmanyPackages];


      char msgArray[howmanyPackages][14];

      for (int i = 0; i < howmanyPackages; i++) {
        howmanyPackagesReceived[i] = 0; // populate array
        sumNumberPackagesNeeded += i + 1;
      }

      //      Serial.print("Number of packages: ");
      //      Serial.println(howmanyPackages);
      //      Serial.println();
      while (1) {
        if (radio.available()) {
          unsigned int sumNumberPackagesReceived = 0;

          radio.read(&msg, sizeof(msg));

          if ((msg[1] == 'i') || (msg[1] == 'f')) {
            if ((String(msg[0])).toInt() != howmanyPackagesReceived[(String(msg[0])).toInt() - 1]) {
              if (strlen(msg) >= 18) {
                lastLenMSG[(String(msg[0])).toInt() - 1] = 16;
              } else {
                lastLenMSG[(String(msg[0])).toInt() - 1] = packageLen - abs(packageLen - int(strlen(msg))); // calculate range of for loop
              }
              //Serial.print(msg[0]); Serial.print(msg[1]);
              for (int i = 2; i < lastLenMSG[(String(msg[0])).toInt() - 1]; i++) {
                msgArray[(String(msg[0])).toInt() - 1][i - 2] = msg[i];
                //Serial.print(msgArray[(String(msg[0])).toInt() - 1][i - 2]);
                //Serial.print(msg[i]);
                //msg[i] = '0';
              }
              //Serial.println();
              //              Serial.print(msg);
              //              Serial.print(" Size: ");
              //              Serial.println(sizeof(msg));
              //              Serial.println(lastLenMSG[(String(msg[0])).toInt() - 1]);
              //              Serial.println(strlen(msg));

              howmanyPackagesReceived[(String(msg[0])).toInt() - 1] = (String(msg[0])).toInt();
              //              Serial.print("howmanyPackagesReceived: ");
              //              Serial.println(howmanyPackagesReceived[(String(msg[0])).toInt() - 1]);
              //              Serial.println();

            }

            for (int i = 0; i < howmanyPackages; i++) {
              sumNumberPackagesReceived += howmanyPackagesReceived[i];
              //                            Serial.print(howmanyPackagesReceived[i]);
              //                            Serial.print(", ");
            }
            //                        Serial.println();
            //            Serial.print("Received packages: ");
            //            Serial.print(sumNumberPackagesReceived);
            //            Serial.print(" Packages needed: ");
            //            Serial.println(sumNumberPackagesNeeded);
            //            Serial.println();

            if (sumNumberPackagesReceived == sumNumberPackagesNeeded) {
              //Serial.print("Received packages: ");
              //Serial.print(sumNumberPackagesReceived);
              //Serial.print(" Packages needed: ");
              //Serial.println(sumNumberPackagesNeeded);
              //Serial.println("break;");
              break;
            }
          }
        }
      }

      //      Serial.println();
      //      Serial.println("##########");
      //msgArray[howmanyPackages][lastLenMSG[howmanyPackages] - 1] = '\0'; // remove weird char in last index when len of msg is not 16
      String strMsg = "";
      //      Serial.print("Received: ");
      for (int i = 0; i < howmanyPackages; i++) {
        for (int j = 0; j < lastLenMSG[i] - 2; j++) {
          //Serial.print(msgArray[i][j]);
          strMsg += msgArray[i][j];
          msgLen += 1;
        }
      }
      msgLen += 1;
      strMsg.toCharArray(charMyContent, msgLen);
      *charSize = msgLen;
      //Serial.println();
      //msgArray[0][0] = '\0';
      //memset(msgArray, 0, packageLen);
    }
  }
}
