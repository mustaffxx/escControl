// arduino micro use 6 CE, 11 CSN
// arduino uno use 9 CE, 10 CSN
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(6, 11); // CE, CSN
const byte address[][6] = {"node1", "node2"};

//
const int packageLen = 16;
char msgC[] = "0,0,0,0,0,0,0,0;0.00,0.00,0.00;0.00,0.00;0.0000000000,0.0000000000,0,0;0,0,0;0;";
char msg[packageLen] = "";
int index = 2;
char header[1] = {'i'};
int headerIndex = 1;

void setup() {
  Serial.begin(9600);

  radio.begin();
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_1MBPS);
  radio.setChannel(119);

  radio.openWritingPipe(address[1]);
  radio.openReadingPipe(1, address[0]);
}

void loop() {
  delay(5);
  radio.stopListening();


  String howmanyPackages = "";
  if ((((sizeof(msgC) / packageLen) + 1) < 10)) {
    howmanyPackages = "tt0" + String(((sizeof(msgC) / packageLen) + 1)) + "tt";
  } else {
    howmanyPackages = "tt" + String(((sizeof(msgC) / packageLen) + 1)) + "tt";
  }
  howmanyPackages.toCharArray(msg, 16);

  radio.write(&msg, sizeof(msg));
  msg[0] = '\0';

  Serial.println();
  Serial.print("Size: "); Serial.print(sizeof(msgC)); Serial.print(" ");
  Serial.println(howmanyPackages);
  for (int i = 0; i <= sizeof(msgC); i++) {
    if (index < packageLen) {
      msg[index] = msgC[i];
      //Serial.print(msg[index - 2]);
      index++;
      if (i == sizeof(msgC)) {
        String tochar;
        tochar = String(headerIndex, HEX);
        tochar += "f";
        msg[0] = tochar[0];
        msg[1] = tochar[1];
        radio.write(&msg, sizeof(msg));
        Serial.print(msg);
        Serial.print(" Size: ");
        Serial.println(sizeof(msg));
      }
    } else {
      String tochar;
      tochar = String(headerIndex, HEX);
      tochar += header[0];
      msg[0] = tochar[0];
      msg[1] = tochar[1];

      msg[packageLen] = '\0';

      Serial.print(msg);
      Serial.print(" Size: ");
      Serial.println(sizeof(msg));

      if (headerIndex < (sizeof(msgC) / packageLen) + 1) {
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
  Serial.println();
  Serial.println();

  //char ms[8] = "ii345678";
  //Serial.print(ms);
  //Serial.print(" Size: ");
  //Serial.println(sizeof(ms));
  //Serial.println(ms[8]);
  //Serial.println();

  //radio.write(&msg, sizeof(msg));
  //Serial.print("Enviado: ");
  //Serial.println(msg);
  //Serial.print("Size: ");
  //Serial.println(sizeof(msg));
  //Serial.println();
  delay(500);
}
