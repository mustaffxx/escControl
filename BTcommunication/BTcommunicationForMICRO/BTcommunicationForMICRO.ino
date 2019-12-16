/* USE THIS CODE FOR ARDUINO MICRO --> Serial1 */
String readString;
char c;
void setup()
{
  pinMode(0, INPUT_PULLUP);// only needed for  JY-MCUY v1.06?
  Serial1.begin(9600);
  Serial1.println("OK then, you first, say something.....");
  Serial1.println("Go on, type something in the space above");
  Serial1.println(" and hit Send or Enter,");
  Serial1.println("then I will repeat it!");
  Serial1.println("");
}

void loop() {
  while (Serial1.available())
  {
    delay(3);
    c = Serial1.read();
    readString += c;
  }// end while
  if (readString.length() > 0)
  {
    Serial1.println(c);
    readString = "";
  }
}
