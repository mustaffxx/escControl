#include "SBUS.h"

#define armButton 3 // INT1

// a SBUS object, which is on hardware
// serial port 1
SBUS x8r(Serial1);

// channel, fail safe, and lost frames data
uint16_t channels[16];
bool failSafe;
bool lostFrame;

//
bool arm = 0;
int arm2 = 0;
bool btst = LOW;

//// SWITCH ////

int buttonStatePrevious = LOW;                      // previousstate of the switch

unsigned long minButtonLongPressDuration = 1000;    // Time we wait before we see the press as a long press
unsigned long buttonLongPressMillis;                // Time in ms when we the button was pressed
bool buttonStateLongPress = false;                  // True if it is a long press

const int intervalButton = 50;                      // Time between two readings of the button state
unsigned long previousButtonMillis;                 // Timestamp of the latest reading

unsigned long buttonPressDuration;                  // Time the button is pressed in ms

//// GENERAL ////

unsigned long currentMillis;
unsigned long lastMillis;

void setup() {
  // begin the SBUS communication
  Serial.begin(9600);
  x8r.begin();

  channels[0] = 960;
  channels[1] = 1019;
  channels[2] = 173;
  channels[3] = 998;
  channels[4] = 173;
  channels[5] = 993;
  channels[6] = 173;
  channels[7] = 173;
  for (int i = 8; i <= 15; i++) {
    channels[i] = 998;
  }

  //
  pinMode(armButton, INPUT);
}

void loop() {
  // look for a good SBUS packet from the receiver
  //if (x8r.read(&channels[0], &failSafe, &lostFrame)) {
  //}
  currentMillis = millis();    // store the current time
  readButtonState();
  //armDisarmMotor();
  armDisarmMotor2();
  //buttonPressed();
  // write the SBUS packet to an SBUS compatible servo
  x8r.write(&channels[0]);

  //print();
  if ((arm2 == 0) || (arm2 == 2)) {
    Serial.println("Neut");
  } else if (arm2 == 1) {
    Serial.println("Arm");
  } else if (arm2 == 3) {
    Serial.println("Disarm");
  } else {}
  delay(10);
}

void buttonPressed() {
  if (digitalRead(armButton) == HIGH) {
    btst = !btst;
    if ((btst == 1) && (digitalRead(armButton))) {
      channels[2] = 173;
      channels[3] = 1812;
    } else if ((btst == 0) && (digitalRead(armButton))) {
      channels[2] = 173;
      channels[3] = 173;
    } else {
      channels[2] = 173;
      channels[3] = 998;
    }
  } else {
    channels[2] = 173;
    channels[3] = 998;
  }
}


void armDisarmMotor() {
  //if ((arm == 1) && (digitalRead(armButton)) && (channels[2] <= 200) && (channels[3] >= 800) ) {
  if ((arm == 1) && (digitalRead(armButton))) {
    channels[2] = 173;
    channels[3] = 1812;

    //} else if ((arm == 0) && (digitalRead(armButton)) && (channels[2] <= 200) && (channels[3] <= 1100) ) {
  } else if ((arm == 0) && (digitalRead(armButton))) {
    channels[2] = 173;
    channels[3] = 173;
  } else {
    channels[2] = 173;
    channels[3] = 998;
  }
}

void armDisarmMotor2() {
  //if ((arm == 1) && (digitalRead(armButton)) && (channels[2] <= 200) && (channels[3] >= 800) ) {
  if (arm2 == 1) {
    channels[2] = 173;
    channels[3] = 1812;

    //} else if ((arm == 0) && (digitalRead(armButton)) && (channels[2] <= 200) && (channels[3] <= 1100) ) {
  } else if ((arm2 == 0) || (arm2 == 2)) {
    channels[2] = 173;
    channels[3] = 998;
  } else if (arm2 == 3) {
    channels[2] = 173;
    channels[3] = 173;
  } else {
    channels[2] = 173;
    channels[3] = 998;
  }
}


void readButtonState() {

  // If the difference in time between the previous reading is larger than intervalButton
  if (currentMillis - previousButtonMillis > intervalButton) {

    // Read the digital value of the button (LOW/HIGH)
    int buttonState = digitalRead(armButton);

    // If the button has been pushed AND
    // If the button wasn't pressed before AND
    // IF there was not already a measurement running to determine how long the button has been pressed
    if (buttonState == HIGH && buttonStatePrevious == LOW && !buttonStateLongPress) {
      buttonLongPressMillis = currentMillis;
      buttonStatePrevious = HIGH;
      //Serial.println("Button pressed");
    }

    // Calculate how long the button has been pressed
    buttonPressDuration = currentMillis - buttonLongPressMillis;

    // If the button is pressed AND
    // If there is no measurement running to determine how long the button is pressed AND
    // If the time the button has been pressed is larger or equal to the time needed for a long press
    if (buttonState == HIGH && !buttonStateLongPress && buttonPressDuration >= minButtonLongPressDuration) {
      buttonStateLongPress = true;
      //Serial.println("Button long pressed");
      //Serial.print("arm: ");
      //Serial.println(arm);
      //arm = !arm;
    }

    // If the button is released AND
    // If the button was pressed before
    if (buttonState == LOW && buttonStatePrevious == HIGH) {
      buttonStatePrevious = LOW;
      buttonStateLongPress = false;
      //Serial.println("Button released");

      // If there is no measurement running to determine how long the button was pressed AND
      // If the time the button has been pressed is smaller than the minimal time needed for a long press
      if (!buttonStateLongPress && buttonPressDuration < minButtonLongPressDuration) {
        //Serial.println("Button pressed shortly");
        arm2++;
        if (arm2 > 3) {
          arm2 = 0;
        }
      }
    }

    // store the current timestamp in previousButtonMillis
    previousButtonMillis = currentMillis;

  }

}



void print() {
  for (int i = 0; i < 8; i++) {
    //Serial.print("\tChannel ");
    //Serial.print(i + 1);
    Serial.print("Channel ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(channels[i]);
    Serial.print("\t");
  }
  Serial.print("lostFrame: ");
  Serial.print(lostFrame);
  Serial.print("\tfailSafe: ");
  Serial.print(failSafe);
  Serial.println("");
}
