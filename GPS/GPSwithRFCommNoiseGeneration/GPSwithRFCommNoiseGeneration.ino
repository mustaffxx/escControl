#include <SPI.h>
//#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(6, 11); // CE, CSN
const byte address[6] = "node1";


struct DataFormat {
  char id[6];
  double lat;
  double lng;
  double alt;
};
DataFormat sent;

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
  strcpy(sent.id, "CBA21");

  radio.write(&sent, sizeof(sent));
  Serial.print("Sent: {\n  ID: ");
  Serial.print(String(sent.id));
  Serial.print("\n  lat: ");
  Serial.print(sent.lat, 6);
  Serial.print("\n  lng: ");
  Serial.print(sent.lng, 6);
  Serial.print("\n  alt: ");
  Serial.print(sent.alt, 6);
  Serial.print("\n }");
  Serial.print("\nSize: ");
  Serial.println(sizeof(sent));
  delay(5000);
}
