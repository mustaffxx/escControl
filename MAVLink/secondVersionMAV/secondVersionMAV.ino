#include <mavlink.h>
#include <SoftwareSerial.h>
SoftwareSerial _MavLinkSerial(9, 10); // PIN 9=Telemetry TX->Pixhawk RX, PIN 10=Telemetry RX->Pixhawk TX


// global
uint8_t _system_id = 255; // id of computer which is sending the command (ground control software has id of 255)
uint8_t _component_id = 2; // seems like it can be any # except the number of what Pixhawk sys_id is
uint8_t _target_system = 1; // Id # of Pixhawk (should be 1)
uint8_t _target_component = 0; // Target component, 0 = all (seems to work with 0 or 1

uint16_t CMD_LONG_command;
uint8_t  CMD_LONG_confirmation;
float CMD_LONG_param1 = 0;

void setup() {
  _MavLinkSerial.begin(57600);
  Serial.begin(57600);

  request_datastream();

}

void loop() {

  MavLink_receive();
  if (Serial.available()) {
    char c = Serial.read();
    Serial.println(c);

    if (c == 'a') {
      Serial.println("\nArming!");
      arm();
    } else if (c == 'd') {
      Serial.println("\nDisarming!");
      disarm();
    }
  }

}

//function called by arduino to read any MAVlink messages sent by serial communication from flight controller to arduino
void MavLink_receive()
{
  mavlink_message_t msg;
  mavlink_status_t status;

  while (_MavLinkSerial.available())
  {
    uint8_t c = _MavLinkSerial.read();

    //Get new message
    if (mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &status))
    {

      //Handle new message from autopilot
      switch (msg.msgid)
      {
        case MAVLINK_MSG_ID_GPS_RAW_INT: // 24
          {
            mavlink_gps_raw_int_t packet;
            mavlink_msg_gps_raw_int_decode(&msg, &packet);
            Serial.println("\n\n--MAVLINK_MSG_ID_GPS_RAW_INT--");
            Serial.print("GPS Fix: "); Serial.println(packet.fix_type);
            Serial.print("GPS Latitude: "); Serial.println(packet.lat);
            Serial.print("GPS Longitude: "); Serial.println(packet.lon);
            Serial.print("GPS Speed: "); Serial.println(packet.vel);
            Serial.print("Sats Visible: "); Serial.println(packet.satellites_visible);
          }
          break;

        case MAVLINK_MSG_ID_HIL_CONTROLS: // 91
          {
            mavlink_hil_controls_t packet;
            mavlink_msg_hil_controls_decode(&msg, &packet);
            Serial.println("\n\n--MAVLINK_MSG_ID_HIL_CONTROLS--");
            Serial.print("Roll: "); Serial.println(packet.roll_ailerons);
            Serial.print("Pitch: "); Serial.println(packet.pitch_elevator);
            Serial.print("Yaw: "); Serial.println(packet.yaw_rudder);
            Serial.print("Throttle: "); Serial.println(packet.throttle);
            Serial.print("mode: "); Serial.println(packet.mode);
            Serial.print("Nav mode: "); Serial.println(packet.nav_mode);
          }
          break;

        case MAVLINK_MSG_ID_ATTITUDE:  // 30
          {
            /* Message decoding: PRIMITIVE
                  mavlink_msg_attitude_decode(const mavlink_message_t* msg, mavlink_attitude_t* attitude)
            */
            mavlink_attitude_t packet;
            mavlink_msg_attitude_decode(&msg, &packet);
            Serial.println("\n\n--MAVLINK_MSG_ID_ATTITUDE--");
            Serial.print("ATTITUDE: ");
            Serial.println(packet.roll);
          }
          break;

        case MAVLINK_MSG_ID_RC_CHANNELS_RAW: // 35
          {
            mavlink_rc_channels_raw_t packet;
            mavlink_msg_rc_channels_raw_decode(&msg, &packet);
            Serial.println("\n\n--MAVLINK_MSG_ID_RC_CHANNELS_RAW--");
            Serial.print("Channel 1: "); Serial.println(packet.chan1_raw);
            Serial.print("Channel 2: "); Serial.println(packet.chan2_raw);
            Serial.print("Channel 3: "); Serial.println(packet.chan3_raw);
            Serial.print("Channel 4: "); Serial.println(packet.chan4_raw);
            Serial.print("Channel 5: "); Serial.println(packet.chan5_raw);
            Serial.print("Channel 6: "); Serial.println(packet.chan6_raw);
            Serial.print("Channel 7: "); Serial.println(packet.chan7_raw);
            Serial.print("Channel 8: "); Serial.println(packet.chan8_raw);
          }
          break;

        case MAVLINK_MSG_ID_NAV_CONTROLLER_OUTPUT: // 62
          {
            mavlink_nav_controller_output_t packet;
            mavlink_msg_nav_controller_output_decode(&msg, &packet);
            Serial.println("\n\n--MAVLINK_MSG_ID_NAV_CONTROLLER_OUTPUT--");
            Serial.print("nav_roll: "); Serial.println(packet.nav_roll);
            Serial.print("nav_pitch: "); Serial.println(packet.nav_pitch);
            Serial.print("alt_error: "); Serial.println(packet.alt_error);
            Serial.print("aspd_error: "); Serial.println(packet.aspd_error);
            Serial.print("xtrack_error: "); Serial.println(packet.xtrack_error);
            Serial.print("nav_bearing: "); Serial.println(packet.nav_bearing);
            Serial.print("target_bearing: "); Serial.println(packet.target_bearing);
            Serial.print("wp_dist: "); Serial.println(packet.wp_dist);
          }
          break;

        case MAVLINK_MSG_ID_RAW_IMU: // 27
          {
            mavlink_raw_imu_t packet;
            mavlink_msg_raw_imu_decode(&msg, &packet);
            Serial.println("\n\n--MAVLINK_MSG_ID_RAW_IMU--");
            Serial.print("time_usec: "); Serial.println((int16_t)packet.time_usec);
            Serial.print("xacc: "); Serial.println(packet.xacc);
            Serial.print("yacc: "); Serial.println(packet.yacc);
            Serial.print("zacc: "); Serial.println(packet.zacc);
            Serial.print("xgyro: "); Serial.println(packet.xgyro);
            Serial.print("ygyro: "); Serial.println(packet.ygyro);
            Serial.print("zgyro: "); Serial.println(packet.zgyro);
            Serial.print("xmag: "); Serial.println(packet.xmag);
            Serial.print("ymag: "); Serial.println(packet.ymag);
            Serial.print("zmag: "); Serial.println(packet.zmag);
          }
          break;

        case MAVLINK_MSG_ID_HEARTBEAT: // 27
          {
            mavlink_heartbeat_t packet;
            mavlink_msg_heartbeat_decode(&msg, &packet);
            Serial.println("\n\n--MAVLINK_MSG_ID_HEARTBEAT--");
            Serial.print("custom_mode: "); Serial.println(packet.custom_mode);
            Serial.print("type: "); Serial.println(packet.type);
            Serial.print("autopilot: "); Serial.println(packet.autopilot);
            Serial.print("base_mode: "); Serial.println(packet.base_mode);
            Serial.print("system_status: "); Serial.println(packet.system_status);
            Serial.print("mavlink_version: "); Serial.println(packet.mavlink_version);

          }
          break;

        default:
          Serial.print("\n[ID: ");
          Serial.print(msg.msgid);
          //Serial.print("], [seq: ");
          //Serial.print(msg.seq);
          Serial.print("]");
          break;
      }
    }
  }
}

void request_datastream() {
  //Request Data from Pixhawk
  uint8_t _req_stream_id = MAV_DATA_STREAM_RAW_SENSORS;
  uint16_t _req_message_rate = 0x01; //number of times per second to request the data in hex
  uint8_t _start_stop = 1; //1 = start, 0 = stop

  // STREAMS that can be requested
  /*
    Definitions are in common.h: enum MAV_DATA_STREAM and more importantly at:
    https://mavlink.io/en/messages/common.html#MAV_DATA_STREAM

    MAV_DATA_STREAM_ALL=0, // Enable all data streams
    MAV_DATA_STREAM_RAW_SENSORS=1, /* Enable IMU_RAW, GPS_RAW, GPS_STATUS packets.
    MAV_DATA_STREAM_EXTENDED_STATUS=2, /* Enable GPS_STATUS, CONTROL_STATUS, AUX_STATUS
    MAV_DATA_STREAM_RC_CHANNELS=3, /* Enable RC_CHANNELS_SCALED, RC_CHANNELS_RAW, SERVO_OUTPUT_RAW
    MAV_DATA_STREAM_RAW_CONTROLLER=4, /* Enable ATTITUDE_CONTROLLER_OUTPUT, POSITION_CONTROLLER_OUTPUT, NAV_CONTROLLER_OUTPUT.
    MAV_DATA_STREAM_POSITION=6, /* Enable LOCAL_POSITION, GLOBAL_POSITION/GLOBAL_POSITION_INT messages.
    MAV_DATA_STREAM_EXTRA1=10, /* Dependent on the autopilot
    MAV_DATA_STREAM_EXTRA2=11, /* Dependent on the autopilot
    MAV_DATA_STREAM_EXTRA3=12, /* Dependent on the autopilot
    MAV_DATA_STREAM_ENUM_END=13,

    Data in PixHawk available in:
    - Battery, amperage and voltage (SYS_STATUS) in MAV_DATA_STREAM_EXTENDED_STATUS
    - Gyro info (IMU_SCALED) in MAV_DATA_STREAM_EXTRA1
  */

  // Initialize the required buffers
  mavlink_message_t msg;
  uint8_t buf[MAVLINK_MAX_PACKET_LEN];

  // Pack the message
  mavlink_msg_request_data_stream_pack(_system_id, _component_id, &msg, _target_system, _target_component, _req_stream_id, _req_message_rate, _start_stop);
  uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);  // Send the message (.write sends as bytes)

  _MavLinkSerial.write(buf, len); //Write data to serial port
}

void Command_long() {

  // Define the system type (see mavlink_types.h for list of possible types)

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
  Command_long();
}
void disarm() {
  CMD_LONG_param1 = 0;
  CMD_LONG_command = MAV_CMD_COMPONENT_ARM_DISARM;
  Command_long();
}
