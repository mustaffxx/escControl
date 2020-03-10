// this code implement a algorithm to send data greater than 32kbps with nrf24l01+pa+lna
// only works with 126 max size array
// lost a (maybe) relevant number of packages
// todo: increase max size

// arduino micro use 6 CE, 11 CSN
// arduino uno use 9 CE, 10 CSN
#include <SPI.h>
//#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(6, 11); // CE, CSN
const byte address[][6] = {"node1", "node2"};

// long message
char msgC[] = "aa0,0,0,0,0,0,0,0;0.00,0.00,0.00;0.00,0.00;0.0000000000,0.0000000000,0,0;0,0,0;0;bb";

void setup() {
  Serial.begin(9600);

  radio.begin();
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_1MBPS);
  radio.setChannel(119);

  radio.openWritingPipe(address[1]);
  radio.openReadingPipe(1, address[0]);
  radio.stopListening();
}

void loop() {
  delay(100);
  writeLongMessage(msgC, radio);
}

void writeLongMessage(char msgC[], RF24 radio) {
  const int packageLen = 16; // fix size of package

  char msg[packageLen] = "";
  int index = 2;
  char header[1] = {'i'};
  int headerIndex = 1;
  String howmanyPackages = "";

  if ((((sizeof(msgC) / packageLen) + 1) < 10)) {
    howmanyPackages = "tt0" + String(int(ceil((double)strlen(msgC) / (packageLen - 2)))) + "tt"; // -2 because 16 is the len of all package and 14 is payload
  } else {
    howmanyPackages = "tt" + String(int(ceil((double)strlen(msgC) / (packageLen - 2)))) + "tt"; // -2 because 16 is the len of all package and 14 is payload
  }
  howmanyPackages.toCharArray(msg, 16);

  radio.write(&msg, sizeof(msg));
  msg[0] = '\0';

  //    Serial.println();
  //    Serial.print("Size: "); Serial.print(strlen(msgC)); Serial.print(" ");
  //    Serial.println(howmanyPackages);
  for (int i = 0; i <= strlen(msgC); i++) {
    //    delay(25);
    if (index < packageLen) {
      msg[index] = msgC[i];
      //Serial.print(msg[index - 2]);
      index++;
      if (i == strlen(msgC)) {
        String tochar;
        tochar = String(headerIndex, HEX);
        tochar += "f";
        msg[0] = tochar[0];
        msg[1] = tochar[1];
        radio.write(&msg, sizeof(msg));
        //        Serial.print(msg);
        //        Serial.print(" Size: ");
        //        Serial.println(strlen(msg));
      }
    } else {
      String tochar;
      tochar = String(headerIndex, HEX);
      tochar += header[0];
      msg[0] = tochar[0];
      msg[1] = tochar[1];

      msg[packageLen] = '\0';

      //      Serial.print(msg);
      //      Serial.print(" Size: ");
      //      Serial.println(strlen(msg));

      if (headerIndex <= int(ceil((double)strlen(msgC) / (packageLen - 2)))) { // -2 because 16 is the len of all package and 14 is payload
        headerIndex++;
      } else {
        headerIndex = 1;
      }

      radio.write(&msg, sizeof(msg));

      index = 2;
      msg[index] = msgC[i];
      //Serial.println(msg[index]);
      index++;
    }
    //Serial.print(msgC[i]);
  }

  index = 2;
  headerIndex = 1;
  msg[0] = '\0';
  Serial.print("Send: ");
  Serial.println(msgC);
}
