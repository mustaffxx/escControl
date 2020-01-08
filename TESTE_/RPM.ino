int pinSensor = 7;
int sensor = 0;


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
  Serial.begin(9600);
  pinMode(pinSensor, INPUT);
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

  Serial.print(" RPM: ");
  Serial.println(rpm);

  //delay(100);
}
