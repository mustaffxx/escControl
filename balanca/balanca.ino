#include "WiFiEsp.h" //INCLUSÃO DA BIBLIOTECA
#include "SoftwareSerial.h"//INCLUSÃO DA BIBLIOTECA

int count = 0;

SoftwareSerial Serial2(9, 10); //PINOS QUE EMULAM A SERIAL, ONDE O PINO 6 É O RX E O PINO 7 É O TX

char ssid[] = "2.4G_ClaroVirtua 150"; //VARIÁVEL QUE ARMAZENA O NOME DA REDE SEM FIO
char pass[] = "18001600";//VARIÁVEL QUE ARMAZENA A SENHA DA REDE SEM FIO

WiFiEspServer server(80); //CONEXÃO REALIZADA NA PORTA 80

String Website, data, XML, Javascript;
int inc = 0;

void javascriptContent() {
  Javascript = "<SCRIPT>\n";
  Javascript += "var xmlHttp=createXmlHttpObject();\n";
  Javascript += "function createXmlHttpObject(){\n";
  Javascript += "if(window.XMLHttpRequest){\n";
  Javascript += "xmlHttp=new XMLHttpRequest();\n";
  Javascript += "}else{\n";
  Javascript += "xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');\n";
  Javascript += "}\n";
  Javascript += "return xmlHttp;\n";
  Javascript += "}\n";
  Javascript += "\n";
  Javascript += "function response(){\n";
  Javascript += "xmlResponse=xmlHttp.responseXML;\n";
  Javascript += "xmldoc = xmlResponse.getElementsByTagName('data');\n";
  Javascript += "message = xmldoc[0].firstChild.nodeValue;\n";
  Javascript += "document.getElementById('div1').innerHTML=message;\n";
  Javascript += "}\n";

  Javascript += "function process(){\n";
  Javascript += "xmlHttp.open('PUT','xml',true);\n";
  Javascript += "xmlHttp.onreadystatechange=response;\n";
  Javascript += "xmlHttp.send(null);\n";
  Javascript += "setTimeout('process()',200);\n";
  Javascript += "}\n";

  Javascript += "</SCRIPT>\n";


}

void WebsiteContent() {
  javascriptContent();

  Website = "<html>\n";
  Website += "<style>\n";
  Website += "#div1{\n";
  Website += "width:400px;\n";
  Website += "margin:0 auto;\n";
  Website += "margin-top:130px;\n";
  Website += "font-size:900%;\n";
  Website += "color:powderblue;\n";
  Website += "}\n";
  Website += "</style>\n";
  Website += "<body onload='process()'>";
  Website += "<div id='div1'>" + data + "</div></body></html>";
  Website += Javascript;
  server.println(Website);
}
void XMLcontent() {

  XML = "<?xml version='1.0'?>";
  XML += "<data>";
  XML += data;
  XML += "</data>";

  server.println(XML);

}


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

  WiFi.mode(WIFI_STA);

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
