#include "SBUS.h"

// a SBUS object, which is on hardware
// serial port 1
SBUS x8r(Serial1);

// channel, fail safe, and lost frames data
uint16_t channels[16];
bool failSafe;
bool lostFrame;

void setup() {
  // begin the SBUS communication
  x8r.begin();
}

void loop() {

  // look for a good SBUS packet from the receiver
  if (x8r.read(&channels[0], &failSafe, &lostFrame)) {

    // write the SBUS packet to an SBUS compatible servo
    x8r.write(&channels[0]);

    //Serial.println(channels[0]);
    print();
  }
}


void print() {
  for (int i = 0; i < 16; i++) {
    //Serial.print("\tChannel ");
    //Serial.print(i + 1);
    Serial.print("-\t");
    Serial.print(channels[i]);
  }
  Serial.print("\tlostFrame: ");
  Serial.print(lostFrame);
  Serial.print("\tfailSafe: ");
  Serial.print(failSafe);
  Serial.println("");
}
