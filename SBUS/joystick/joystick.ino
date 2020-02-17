#include "SBUS.h"

// a SBUS object, which is on hardware
// serial port 1
SBUS x8r(Serial1);

// channel, fail safe, and lost frames data
uint16_t channels[16];
bool failSafe;
bool lostFrame;

unsigned long times;


// joystick
#define xLeft A0
#define yLeft A1
#define xRight A2
#define yRight A3

void setup() {
  // begin the SBUS communication
  x8r.begin();

  // fill array of standard values
  channels[4] = 173;
  channels[5] = 173;
  channels[6] = 173;
  channels[7] = 173;
  for (int i = 8; i <= 15; i++) {
    channels[i] = 1187;
  }

  //
  pinMode(xLeft, INPUT);
  pinMode(yLeft, INPUT);
  pinMode(xRight, INPUT);
  pinMode(yRight, INPUT);
}

void loop() {
  // look for a good SBUS packet from the receiver
  if (x8r.read(&channels[0], &failSafe, &lostFrame)) { // if received sbus data

    readJoystick();
    // write the SBUS packet to an SBUS compatible servo
    x8r.write(&channels[0]);

    //Serial.println(channels[0]);
    print();
  } else {
    readJoystick();
    // write the SBUS packet to an SBUS compatible servo
    x8r.write(&channels[0]);
    print();
    delay(7);
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

void readJoystick() {
  int xl, yl, xr, yr;
  xl = map(analogRead(xLeft), 0, 1023, 173, 1811);
  yl = map(analogRead(yLeft), 0, 1023, 173, 1811);
  xr = map(analogRead(xRight), 0, 1023, 173, 1811);
  yr = map(analogRead(yRight), 0, 1023, 173, 1811);

  //
  channels[3] = yl;
  channels[2] = xl;
  channels[1] = yr;
  channels[0] = xr;

  //
  //Serial.print("Left: "); Serial.print(xl); Serial.print("\t-\t"); Serial.print(yl);
  //Serial.print("\tRight: "); Serial.print(xr); Serial.print("\t-\t"); Serial.println(yr);

}
