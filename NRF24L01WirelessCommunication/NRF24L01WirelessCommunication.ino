void setup() {
  Serial.begin(9600);
}

void loop() {

  if(Serial.available() > 0){
    byte bRead = Serial.read();
    Serial.println(bRead);
    }
  delay(50);
}
