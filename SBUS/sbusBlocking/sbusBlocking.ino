#include <sbus.h>
#include <Servo.h>

// used pins
#define SBUS_PIN 9   // D3
#define SERVO_PIN 6  // D2

SBUS sbus;
Servo servo;

void setup() {
  Serial.begin(115200);
  sbus.begin(SBUS_PIN, sbusBlocking);
  servo.attach(SERVO_PIN);
}


void loop() {

  if (!sbus.waitFrame()) {

    Serial.println("Timeout!");

  } else {

    for (int i = 1; i <= 18; ++i) {
      Serial.print(sbus.getChannel(i));
      Serial.print(" ");
      servo.writeMicroseconds(sbus.getChannel(1));
    }

    if (sbus.signalLossActive())
      Serial.print("SIGNAL_LOSS ");

    if (sbus.failsafeActive())
      Serial.print("FAILSAFE");

    Serial.println();

  }

}
