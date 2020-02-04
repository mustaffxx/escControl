#include <mavlink.h>
#include <SoftwareSerial.h>
SoftwareSerial _MavLinkSerial(9, 10); // PIN 9=Telemetry TX->Pixhawk RX, PIN 10=Telemetry RX->Pixhawk TX

// Mavlink variables
unsigned long previousMillisMAVLink = 0;     // will store last time MAVLink was transmitted and listened
unsigned long next_interval_MAVLink = 1000;  // next interval to count
const int num_hbs = 60;                      // # of heartbeats to wait before activating STREAMS from Pixhawk. 60 = one minute.
int num_hbs_pasados = num_hbs;

void setup() {
  // MAVLink interface start
  _MavLinkSerial.begin(57600);

  Serial.begin(9600);
  Serial.println("MAVLink starting.");
}

void loop() {

  // MAVLink
  /* The default UART header for your MCU */
  int sysid = 1;                   ///< ID 20 for this airplane. 1 PX, 255 ground station
  int compid = 158;                ///< The component sending the message
  int type = MAV_TYPE_QUADROTOR;   ///< This system is an airplane / fixed wing

  // Define the system type, in this case an airplane -> on-board controller
  // uint8_t system_type = MAV_TYPE_FIXED_WING;
  uint8_t system_type = MAV_TYPE_GENERIC;
  uint8_t autopilot_type = MAV_AUTOPILOT_INVALID;

  uint8_t system_mode = MAV_MODE_PREFLIGHT; ///< Booting up
  uint32_t custom_mode = 0;                 ///< Custom mode, can be defined by user/adopter
  uint8_t system_state = MAV_STATE_STANDBY; ///< System ready for flight

  // Initialize the required buffers
  mavlink_message_t msg;
  uint8_t buf[MAVLINK_MAX_PACKET_LEN];

  // Pack the message
  //mavlink_msg_heartbeat_pack(sysid,compid, &msg, type, autopilot_type, system_mode, custom_mode, system_state);
  mavlink_msg_heartbeat_pack(1, 0, &msg, type, autopilot_type, system_mode, custom_mode, system_state);

  // Copy the message to the send buffer
  uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);

  // Send the message with the standard UART send function
  // uart0_send might be named differently depending on
  // the individual microcontroller / library in use.
  unsigned long currentMillisMAVLink = millis();
  if (currentMillisMAVLink - previousMillisMAVLink >= next_interval_MAVLink)
  {
    // Record last HB update
    previousMillisMAVLink = currentMillisMAVLink;


    //Mav_Request_Data();
    num_hbs_pasados++;
    if (num_hbs_pasados >= num_hbs) {
      // Request streams from Pixhawk
      Serial.println("Streams requested!");
      Mav_Request_Data();
      num_hbs_pasados = 0;
    }

  }

  // Check reception buffer
  comm_receive();
}


void Mav_Request_Data()
{
  mavlink_message_t msg;
  uint8_t buf[MAVLINK_MAX_PACKET_LEN];


  // To be setup according to the needed information to be requested from the Pixhawk
  const int  maxStreams = 1;
  const uint8_t MAVStreams[maxStreams] = {MAV_DATA_STREAM_POSITION};
  const uint16_t MAVRates[maxStreams] = {0x02};

  for (int i = 0; i < maxStreams; i++) {
    mavlink_msg_request_data_stream_pack(6, 200, &msg, 1, 0, MAVStreams[i], MAVRates[i], 1);
    uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);
    _MavLinkSerial.write(buf, len);
  }
}

void comm_receive() {
  mavlink_message_t msg;
  mavlink_status_t status;

  while (_MavLinkSerial.available() > 0) {
    uint8_t c = _MavLinkSerial.read();

    // Try to get a new message
    if (mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &status))
    {
      //Serial.print("DEBUG msgid:");
      //Serial.println(msg.msgid);
      // Handle message
      switch (msg.msgid)
      {
        case MAVLINK_MSG_ID_HEARTBEAT:  // #0: Heartbeat
          {
            // E.g. read GCS heartbeat and go into
            // comm lost mode if timer times out
            Serial.println("PX HB");
          }
          break;
        case MAVLINK_MSG_ID_SYS_STATUS:  // #1: SYS_STATUS
          {
            /* Message decoding: PRIMITIVE
                  mavlink_msg_sys_status_decode(const mavlink_message_t* msg, mavlink_sys_status_t* sys_status)
            */
            //mavlink_message_t* msg;
            mavlink_sys_status_t sys_status;
            mavlink_msg_sys_status_decode(&msg, &sys_status);
            Serial.print("PX SYS STATUS: ");
            Serial.print("[Bat (V): ");
            Serial.print(sys_status.voltage_battery);
            Serial.print("], [Bat (A): ");
            Serial.print(sys_status.current_battery);
            Serial.print("], [Comms loss (%): ");
            Serial.print(sys_status.drop_rate_comm);
            Serial.println("]");
          }
          break;

        case MAVLINK_MSG_ID_PARAM_VALUE:  // #22: PARAM_VALUE
          {
            /* Message decoding: PRIMITIVE
                  mavlink_msg_param_value_decode(const mavlink_message_t* msg, mavlink_param_value_t* param_value)
            */
            //mavlink_message_t* msg;
            mavlink_param_value_t param_value;
            mavlink_msg_param_value_decode(&msg, &param_value);
            Serial.println("PX PARAM_VALUE");
            Serial.println(param_value.param_value);
            Serial.println(param_value.param_count);
            Serial.println(param_value.param_index);
            Serial.println(param_value.param_id);
            Serial.println(param_value.param_type);
            Serial.println("------ Fin -------");
          }
          break;

        case MAVLINK_MSG_ID_RAW_IMU:  // #27: RAW_IMU
          {
            /* Message decoding: PRIMITIVE
                  static inline void mavlink_msg_raw_imu_decode(const mavlink_message_t* msg, mavlink_raw_imu_t* raw_imu)
            */
            mavlink_raw_imu_t raw_imu;
            mavlink_msg_raw_imu_decode(&msg, &raw_imu);
            Serial.println("PX RAW IMU");
            Serial.println(raw_imu.xacc);
          }
          break;

        case MAVLINK_MSG_ID_ATTITUDE:  // #30
          {
            /* Message decoding: PRIMITIVE
                  mavlink_msg_attitude_decode(const mavlink_message_t* msg, mavlink_attitude_t* attitude)
            */
            mavlink_attitude_t attitude;
            mavlink_msg_attitude_decode(&msg, &attitude);
            Serial.println("PX ATTITUDE");
            Serial.println(attitude.roll);
            //if (attitude.roll > 1) leds_modo = 0;
            //else if (attitude.roll < -1) leds_modo = 2;
            //else leds_modo = 1;
          }
          break;

        default:
          Serial.print("--- Otros: ");
          Serial.print("[ID: ");
          Serial.print(msg.msgid);
          Serial.print("], [seq: ");
          Serial.print(msg.seq);
          Serial.println("]");
          break;
      }
    }
  }
}
