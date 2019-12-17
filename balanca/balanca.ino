#include "WiFiEsp.h" //INCLUSÃO DA BIBLIOTECA
#include "SoftwareSerial.h"//INCLUSÃO DA BIBLIOTECA

int ledPin = 3;
int count = 0;

SoftwareSerial Serial2(9, 10); //PINOS QUE EMULAM A SERIAL, ONDE O PINO 6 É O RX E O PINO 7 É O TX

char ssid[] = "xxxx"; //VARIÁVEL QUE ARMAZENA O NOME DA REDE SEM FIO
char pass[] = "yyyy";//VARIÁVEL QUE ARMAZENA A SENHA DA REDE SEM FIO

WiFiEspServer server(80); //CONEXÃO REALIZADA NA PORTA 80

void setup() {
  Serial.begin(9600); //INICIALIZA A SERIAL
  Serial2.begin(9600); //INICIALIZA A SERIAL PARA O ESP8266
  WiFi.init(&Serial2); //INICIALIZA A COMUNICAÇÃO SERIAL COM O ESP8266
  WiFi.config(IPAddress(192, 168, 0, 25)); //COLOQUE UMA FAIXA DE IP DISPONÍVEL DO SEU ROTEADOR

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  WiFi.begin(ssid, pass);

  if (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  server.begin();
  Serial.println("Server started");
  Serial.print("URL: http://");
  Serial.print(WiFi.localIP());
  Serial.println("");
}

void loop() {
  WiFiEspClient client = server.available(); //ATENDE AS SOLICITAÇÕES DO CLIENTE

  if (client) {
    // Wait until the client sends some data
    Serial.println("\nNew client");

    /*while (!client.available()) {
      delay(1);
      }*/

    while (client.connected()) {
      if (client.available()) {
        // Read the first line of the request
        String request = client.readStringUntil('\r');
        Serial.println(request);
        client.flush();

        // Return the response
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println(""); //  do not forget this one
        client.println("<!DOCTYPE HTML>");
        client.println("<html>");
        client.println("<HEAD><meta http-equiv=\"refresh\" content=\"5\"></HEAD>");

        client.println("Contador: ");
        client.println(count);
        client.println("</html>");

        delay(1);
        //Serial.println("Client disconnected");
        Serial.println("");

        count += 1;
        client.stop();
      }
    }
  }
}
