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
bool arm = LOW;

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
  currentMillis = millis();    // store the current time

  // look for a good SBUS packet from the receiver
  if (x8r.read(&channels[0], &failSafe, &lostFrame)) {
    x8r.write(&channels[0]);
    readButtonState();
  } else {
    readButtonState();
  }
  print();
}


int readButtonState() {
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
        if (arm == LOW) {
          Serial.println("Arming...");
          unsigned long timeArm = millis();
          while ((millis() - timeArm) < 5000) {
            channels[2] = 173;
            channels[3] = 1812;
            x8r.write(&channels[0]);
            delay(10);
          }
          arm = HIGH;
          Serial.println("Armed!\n");
        } else if (arm == HIGH) {
          Serial.println("Disarming...");
          unsigned long timeArm = millis();
          while ((millis() - timeArm) < 5000) {
            channels[2] = 173;
            channels[3] = 173;
            x8r.write(&channels[0]);
            delay(10);
          }
          arm = LOW;
          Serial.println("Disarmed!\n");
        }
        // return
        channels[2] = 173;
        channels[3] = 998;
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
