#include <Servo.h>

Servo esc;

int r = 0;

void setup() {
  Serial.begin(9600);
  esc.attach(10, 1000, 2000);
}

void loop() {
  if (Serial.available() > 0) {
    r = Serial.parseInt();
    if (r != 0) {
      Serial.print("Received: ");
      Serial.print(r);
      r = map(r, 1, 100, 1000, 2000);
      if (r < 1000) {
        r = 1000;
      }
      if (r > 2000) {
        r = 1000;
      }
      esc.write(r);
      Serial.print(" ESC: ");
      Serial.println(r);
    }
  }
}
