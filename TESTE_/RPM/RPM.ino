#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

int pinSensor = 7;
int sensor = 0;

//radio
RF24 radio(6, 11); // CE, CSN
const byte address2[6] = "node2";

//delay send
int period = 100;
unsigned long time_now = 0;

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
  pinMode(pinSensor, INPUT);

  //radio
  radio.begin();
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_2MBPS);
  radio.setChannel(125);
  radio.openWritingPipe(address2);
  radio.stopListening();
}

void loop() {

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
      }

      prevTime = currentTime;
    }
    state2 = state1;
  }

  if (millis() > time_now + period) {
    time_now = millis();
    radio.write(&rpm , sizeof(rpm));
  }
}
