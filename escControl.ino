#include <Servo.h>

#define pot A0
Servo esc;

int read = 0;
int speed = 0;

void setup() {
  Serial.begin(9600);
  pinMode(pot, INPUT);
  
  esc.attach(10);
  esc.writeMicroseconds(2000);
  delay(2000);
  esc.writeMicroseconds(1000);
  delay(300);
}

void loop() {
  read = analogRead(pot);
  speed = map(read, 0, 1023, 1000, 2000);
  Serial.print("Potênciometro: ");
  Serial.print(read);
  Serial.print(" writeMicroseconds: ");
  Serial.println(speed);
  esc.writeMicroseconds(speed);
  delay(50);
}
