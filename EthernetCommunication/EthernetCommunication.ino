#include <UIPEthernet.h> //INCLUSÃO DE BIBLIOTECA
#include <Servo.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //ATRIBUIÇÃO DE ENDEREÇO MAC AO ENC28J60
byte ip[] = { 192, 168, 0, 25 };
EthernetServer server(80);

Servo esc;
const int escPin = 6;
int spd = 0;
int spdEsc = 0;

String readString;

void setup() {
  Serial.begin(9600);
  esc.attach(escPin);
  delay(1000);

  Ethernet.begin(mac, ip);
  server.begin();

  
}
void loop() {


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
          client.println("<TITLE>escControl</TITLE>");
          client.println("<meta http-equiv=\"refresh\" content=\"1\">");

          client.println("</HEAD>");
          client.println("<body style=background-color:#ADD8E6><center>");
          client.println("<H1>escControl</H1>");
          client.println("<input type=\"button\" onclick=\"location.href='?speed-';\" value=\"Decrease\" />");
          client.println("<input type=\"button\" onclick=\"location.href='?speed+';\" value=\"Increase\" />");
          //client.println("<a href=\"/?light-\"> - </a> <a href=\"/?light+\"> + </a>");
          client.println(" </center></BODY></HTML>");
          delay(1);
          client.stop();

          if (readString.indexOf("?speed-") > 0) //checks for -
          { spd = spd - 5;
            if (spd < 0) {
              spd = 0;
            }
            spdEsc = map(spd, 0, 100, 1000, 2000);
            esc.writeMicroseconds(spdEsc);
            
            Serial.println("spd -");
            Serial.println(spdEsc);

            delay(1);
            client.stop();
            readString = "";
          }
          else
          {
            if (readString.indexOf("?speed+") > 0) //checks for +
            { spd = spd + 5;
              if (spd > 100) {
                spd = 100;
              }
              spdEsc = map(spd, 0, 100, 1000, 2000);
              esc.writeMicroseconds(spdEsc);

              Serial.println("spd +");
              Serial.println(spdEsc);

              delay(1);
              client.stop();
              readString = "";
            }

            delay(1);
            client.stop();
            readString = "";
          }
        }
      }
    }
  }
}
