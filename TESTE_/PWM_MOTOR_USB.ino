#include <Servo.h>

int pinSensor = 7;
int sensor = 0;

//
int asciRead = 0;
int spdPWM = 1000;
int spd = 0;
int spdAnterior = 1000;

//
Servo esc;

//rpm
int sensorvalue;
int state1 = HIGH;
int state2;
float rps;
float rpm;
long prevMillis = 0;
long interval = 200;
long currentTime;
long prevTime = 1;
long diffTime;
int sensorthreshold = 30;

void setup() {
  esc.attach(5);
  delay(300);
  Serial.begin(9600);
  pinMode(pinSensor, INPUT);
}

void loop() {
  asciRead = 0;
  if (Serial.available() > 0) {
    asciRead = Serial.read();
  }
  setSpeed(asciRead);
  spdPWM = map(spd, 0, 100, 1000, 2000);
  if (spdPWM != spdAnterior) {
    esc.writeMicroseconds(spdPWM);
    Serial.println("Alterou");
  }
  /*
    if (digitalRead(pinSensor) == LOW) {
    sensor = 1;
    } else {
    sensor = 0;
    }*/

  //rpm
  sensorvalue = digitalRead(pinSensor); // read from pin 0
  if (sensorvalue == LOW)
    state1 = HIGH;
  else
    state1 = LOW;
  //digitalWrite(13, state1);   // as iR light is invisible for us, the led on pin 13
  // indicate the state of the circuit.

  if (state2 != state1) {  //counts when the state change, thats from (dark to light) or
    //from (light to dark), remember that IR light is invisible to us.
    if (state2 > state1) {
      currentTime = micros();   // Get the arduino time in microseconds
      diffTime = currentTime - prevTime;  // calculate the time diff from the last meet-up
      rps = 1000000 / diffTime; // calculate how many rev per second, good to know
      rpm = 60000000 / diffTime; // calculate how many rev per minute

      unsigned long currentMillis = millis();

      // print to serial at every interval - defined at the variables declaration
      if (currentMillis - prevMillis > interval) { // see if now already an interval long
        prevMillis = currentMillis;
        Serial.print(rps); Serial.print(" rps  "); Serial.print(rpm); Serial.println(" rpm");
      }

      prevTime = currentTime;
    }
    state2 = state1;
  }

  /*
    //only for testing to determine the sensorthreshold value
    delay(500);
    Serial.println(sensorvalue);
  */

  
  Serial.print("Velocidade: ");
  Serial.print(spd);
  Serial.print("% microsegundos: ");
  Serial.println(spdPWM);
  //Serial.print(" Sensor: ");
  //Serial.print(sensor);
  //Serial.print(" RPM: ");
  //Serial.println(rpm);
  spdAnterior = spdPWM;
  
  //delay(100);
}


void setSpeed(int asciRead) {
  switch (asciRead) {
    case 122:
      spd = 0;
      break;
    case 90:
      spd = 0;
      break;
    case 48:
      spd = 5;
      break;
    case 49:
      spd = 10;
      break;
    case 50:
      spd = 20;
      break;
    case 51:
      spd = 30;
      break;
    case 52:
      spd = 40;
      break;
    case 53:
      spd = 50;
      break;
    case 54:
      spd = 60;
      break;
    case 55:
      spd = 70;
      break;
    case 56:
      spd = 80;
      break;
    case 57:
      spd = 90;
      break;
    case 109:
      spd = 100;
      break;
    case 77:
      spd = 100;
      break;
    case 43:
      spd = spd + 5;
      break;
    case 45:
      spd = spd - 5;
      break;
    default:
      break;
  }
  if (spd < 0) spd = 0;
  if (spd > 100) spd = 100;
}
