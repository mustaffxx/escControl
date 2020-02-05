#include <mavlink.h>
#include <SoftwareSerial.h>
SoftwareSerial _MavLinkSerial(9, 10); // PIN 9 = Telemetry TX -> Pixhawk RX, PIN 10 = Telemetry RX -> Pixhawk TX

#define armButton 3
#define led 5

// global
uint8_t _system_id = 255; // id of computer which is sending the command (ground control software has id of 255)
uint8_t _component_id = 2; // seems like it can be any # except the number of what Pixhawk sys_id is
uint8_t _target_system = 1; // Id # of Pixhawk (should be 1)
uint8_t _target_component = 0; // Target component, 0 = all (seems to work with 0 or 1

uint16_t CMD_LONG_command;
uint8_t  CMD_LONG_confirmation;
float CMD_LONG_param1 = 0;

//// GENERAL ////
unsigned long currentMillis;
unsigned long lastMillis;

// button
int buttonStatePrevious = LOW;                      // previousstate of the switch
unsigned long minButtonLongPressDuration = 1000;    // Time we wait before we see the press as a long press
unsigned long buttonLongPressMillis;                // Time in ms when we the button was pressed
bool buttonStateLongPress = false;                  // True if it is a long press
const int intervalButton = 50;                      // Time between two readings of the button state
unsigned long previousButtonMillis;                 // Timestamp of the latest reading
unsigned long buttonPressDuration;                  // Time the button is pressed in ms
bool state = LOW;

void setup() {
  pinMode(armButton, INPUT);
  pinMode(led, OUTPUT);

  _MavLinkSerial.begin(57600);
  Serial.begin(57600);
}

void loop() {
  currentMillis = millis();
  readButtonState();
}

void send_command() {
  // Initialize the required buffers
  mavlink_message_t msg;
  uint8_t buf[MAVLINK_MAX_PACKET_LEN];

  // Pack the message
  //uint16_t mavlink_msg_command_long_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,uint8_t target_system, uint8_t target_component, uint16_t command, uint8_t confirmation, float param1, float param2, float param3, float param4, float param5, float param6, float param7)
  mavlink_msg_command_long_pack(_system_id, _component_id, &msg, _target_system, _target_component, CMD_LONG_command, CMD_LONG_confirmation, CMD_LONG_param1, 0, 0, 0, 0, 0, 0);

  // Copy the message to send buffer
  uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);

  // Send the message (.write sends as bytes)
  _MavLinkSerial.write(buf, len);
}
void arm() {
  CMD_LONG_param1 = 1;
  CMD_LONG_command = MAV_CMD_COMPONENT_ARM_DISARM;
  send_command();
  Serial.println("\nArming!");
  digitalWrite(led, HIGH);
}
void disarm() {
  CMD_LONG_param1 = 0;
  CMD_LONG_command = MAV_CMD_COMPONENT_ARM_DISARM;
  send_command();
  Serial.println("\nDisarming!");
  digitalWrite(led, LOW);
}

void readButtonState() {
  // If the difference in time between the previous reading is larger than intervalButton
  if (currentMillis - previousButtonMillis > intervalButton) {
    // Read the digital value of the button (LOW/HIGH)
    int buttonState = digitalRead(armButton);
    // If the button has been pushed AND
    // If the button wasn't pressed before AND
    // IF there was not already a measurement running to determine how long the button has been pressed
    if (buttonState == HIGH && buttonStatePrevious == LOW && !buttonStateLongPress) {
      buttonLongPressMillis = currentMillis;
      buttonStatePrevious = HIGH;
      //Serial.println("Button pressed");
    }
    // Calculate how long the button has been pressed
    buttonPressDuration = currentMillis - buttonLongPressMillis;

    // If the button is pressed AND
    // If there is no measurement running to determine how long the button is pressed AND
    // If the time the button has been pressed is larger or equal to the time needed for a long press
    if (buttonState == HIGH && !buttonStateLongPress && buttonPressDuration >= minButtonLongPressDuration) {
      buttonStateLongPress = true;
      //Serial.println("Button long pressed");
    }

    // If the button is released AND
    // If the button was pressed before
    if (buttonState == LOW && buttonStatePrevious == HIGH) {
      buttonStatePrevious = LOW;
      buttonStateLongPress = false;
      //Serial.println("Button released");

      // If there is no measurement running to determine how long the button was pressed AND
      // If the time the button has been pressed is smaller than the minimal time needed for a long press
      if (!buttonStateLongPress && buttonPressDuration < minButtonLongPressDuration) {
        //Serial.println("Button pressed shortly");
        if (state == HIGH) {
          arm();
          state = !state;
        } else {
          disarm();
          state = !state;
        }
      }
    }
    // store the current timestamp in previousButtonMillis
    previousButtonMillis = currentMillis;
  }
}
