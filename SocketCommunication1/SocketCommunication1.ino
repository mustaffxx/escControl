#include "WiFiEsp.h"
#include "SoftwareSerial.h"
#include <WebSocketServer.h>
#include <ArduinoJson.h>

SoftwareSerial Serial1(6, 7);

WiFiEspServer server(80);
WebSocketServer webSocketServer;

const char* ssid = "XXXX";
const char* password =  "YYYY";

void setup() {

  Serial.begin(9600);
  Serial1.begin(9600);
  WiFi.init(&Serial1);
  //WiFi.config(IPAddress(192, 168, 0, 25));

  delay(2000);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());

  server.begin();
  delay(100);
}

void loop() {

  WiFiEspClient client = server.available();

  if (client.connected() && webSocketServer.handshake(client)) {

    String data;

    while (client.connected()) {

      data = webSocketServer.getData();

      if (data.length() > 0) {
        Serial.println(data);
        webSocketServer.sendData(data);
      }

      delay(10); // Delay needed for receiving the data correctly
    }

    Serial.println("The client disconnected");
    delay(100);
  }

  delay(100);
}
