#include <UIPEthernet.h> //INCLUSÃO DE BIBLIOTECA

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //ATRIBUIÇÃO DE ENDEREÇO MAC AO ENC28J60
byte ip[] = { 192, 168, 0, 25 };
EthernetServer server(80);

int ledPin = 8;

String readString;
int status = 0;
int pos1 = 0;

void setup() {
  Ethernet.begin(mac, ip);
  server.begin();
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}
void loop() {

  byte brightness;
  EthernetClient client = server.available();
  if (client) { //
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (readString.length() < 100)
        {
          readString += c; // "readstring" VAI RECEBER OS CARACTERES LIDO
        }
        if (c == '\n') {
          Serial.println(readString);

          client.println("HTTP/1.1 200 OK");//send new page
          client.println("Content-Type: text/html");
          client.println();
          client.println("<HTML>");
          client.println("<HEAD>");
          client.println("<TITLE>Arduuino Project</TITLE>");
          client.println("<meta http-equiv=\"refresh\" content=\"1\">");

          client.println("</HEAD>");
          client.println("<BODY bgcolor='white'><center>");
          client.println("<H1 style=\"background-color:white;\">Arduino project</H1>");

          client.println("<a href=\"/?light-\"> - </a> <a href=\"/?light+\"> + </a>");
          client.println(" </center></BODY></HTML>");
          delay(1);
          //stopping client
          client.stop();
          /////////////////////


          if (readString.indexOf("?light-") > 0) //checks for -
          { brightness = brightness - 15;
            analogWrite(9, brightness);
            Serial.println("brightness -");
          }
          else
          {
            if (readString.indexOf("?light+") > 0) //checks for +
            { brightness = brightness + 15;
              analogWrite(9, brightness);
              Serial.println("brightness +");
            }


            delay(1);
            //stopping client
            client.stop();
            readString = "";

          }
        }
      }
    }
  }
}
