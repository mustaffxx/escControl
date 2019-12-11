#include "WiFiEsp.h"
#include "SoftwareSerial.h"
#include <WebSocketServer.h>
#include <ArduinoJson.h>

SoftwareSerial Serial1(6, 7);

WiFiEspServer server(80);
WebSocketServer webSocketServer;

const char* ssid = "XXXX";
const char* password =  "YYYY";

void handleReceivedMessage(String message) {

  StaticJsonDocument<500> JSONBuffer;                     //Memory pool
  auto error = deserializeJson(JSONBuffer, message); //Parse message

  if (error) {   //Check for errors in parsing
    Serial.println("Parsing failed");
    return;
  }

  const char * sensorType = JSONBuffer["sensor"];           //Get sensor type value
  const char * sensorIdentifier = JSONBuffer["identifier"]; //Get sensor type value
  const char * timestamp = JSONBuffer["timestamp"];         //Get timestamp
  int value = JSONBuffer["value"];                          //Get value of sensor measurement

  Serial.println();
  Serial.println("----- NEW DATA FROM CLIENT ----");

  Serial.print("Sensor type: ");
  Serial.println(sensorType);

  Serial.print("Sensor identifier: ");
  Serial.println(sensorIdentifier);

  Serial.print("Timestamp: ");
  Serial.println(timestamp);

  Serial.print("Sensor value: ");
  Serial.println(value);

  Serial.println("------------------------------");
}

void setup() {

  Serial.begin(9600);
  Serial1.begin(9600);
  WiFi.init(&Serial1);
  WiFi.config(IPAddress(192, 168, 0, 25));

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
        handleReceivedMessage(data);
        webSocketServer.sendData(data);
      }

      delay(10); // Delay needed for receiving the data correctly
    }

    Serial.println("The client disconnected");
    delay(100);
  }

  delay(100);
}
