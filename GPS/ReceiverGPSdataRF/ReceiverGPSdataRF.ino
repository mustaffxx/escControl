#include <SPI.h>
#include <RF24.h>

RF24 radio(6, 11); // CE, CSN
const byte address[6] = "node1";

//
struct DataFormat {
  char id[6];
  double lat;
  double lng;
  double alt;
};
DataFormat received;

void setup() {
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
    Serial.print("\nReceived: {\n  ID: ");
    Serial.print(String(received.id));
    Serial.print("\n  lat: ");
    Serial.print(received.lat, 6);
    Serial.print("\n  lng: ");
    Serial.print(received.lng, 6);
    Serial.print("\n  alt: ");
    Serial.print(received.alt, 6);
    Serial.print("\n }");
    Serial.print("\nSize: ");
    Serial.println(sizeof(received));
  }
}
