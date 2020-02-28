#include <mavlink.h>
#include <SoftwareSerial.h>
SoftwareSerial _MavLinkSerial(9, 10); // PIN 9=Telemetry TX->Pixhawk RX, PIN 10=Telemetry RX->Pixhawk TX

// global
uint8_t _system_id = 255; // id of computer which is sending the command (ground control software has id of 255)
uint8_t _component_id = 2; // seems like it can be any # except the number of what Pixhawk sys_id is
uint8_t _target_system = 1; // Id # of Pixhawk (should be 1)
uint8_t _target_component = 0; // Target component, 0 = all (seems to work with 0 or 1

/* for future
  uint16_t CMD_LONG_command;
  uint8_t  CMD_LONG_confirmation;
  float CMD_LONG_param1 = 0;
*/

// mavlink structs
mavlink_heartbeat_t heart;
bool hbmotor_armed = 0; // heartbeat aux
String hbstatus = ""; // heartbeat aux
mavlink_gps_raw_int_t gps;
//mavlink_hil_controls_t hil;
mavlink_attitude_t attitude;
mavlink_rc_channels_raw_t channels;
mavlink_vfr_hud_t vrf;
mavlink_sys_status_t sys_status;
mavlink_statustext_t statustext;

// motor
//bool motor_armed = LOW;

// joystick
#define xLeft A0
#define yLeft A1
#define xRight A2
#define yRight A3

// middleware check if channels varied
int oldChannels[4] = {0, 0, 0, 0};

bool stateled = 0;
void setup() {
  _MavLinkSerial.begin(57600);
  Serial.begin(57600);

  requestDatastream();
  digitalWrite(13, LOW); // led bultin to see if code still running
}

void loop() {
  /* for future
    if (Serial.available()) {
    char c = Serial.read();
    //Serial.println(c);

    if (c == 'a') {
      Serial.println("\nArming!");
      arm();
    } else if (c == 'd') {
      Serial.println("\nDisarming!");
      disarm();
    }
    }
  */
  readJoystick();
  readMavlink();
  printInfo();
  stateled = !stateled; // led bultin to see if code still running
  digitalWrite(13, stateled); // led bultin to see if code still running
  delay(100);
}

//function called by arduino to read any MAVlink messages sent by serial communication from flight controller to arduino
void readMavlink()
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
            mavlink_msg_gps_raw_int_decode(&msg, &gps);
          }
          break;

        /*
          case MAVLINK_MSG_ID_HIL_CONTROLS: // 91
          {
            mavlink_msg_hil_controls_decode(&msg, &hil);
            Serial.println("\n\n--MAVLINK_MSG_ID_HIL_CONTROLS--");
            Serial.print("Roll: "); Serial.println(packet.roll_ailerons);
            Serial.print("Pitch: "); Serial.println(packet.pitch_elevator);
            Serial.print("Yaw: "); Serial.println(packet.yaw_rudder);
            Serial.print("Throttle: "); Serial.println(packet.throttle);
            Serial.print("mode: "); Serial.println(packet.mode);
            Serial.print("Nav mode: "); Serial.println(packet.nav_mode);
          }
          break;
        */

        case MAVLINK_MSG_ID_ATTITUDE:  // 30
          {
            mavlink_msg_attitude_decode(&msg, &attitude);
          }
          break;

        case MAVLINK_MSG_ID_SYS_STATUS:
          {
            mavlink_msg_sys_status_decode(&msg, &sys_status);
          }
          break;

        case MAVLINK_MSG_ID_RC_CHANNELS_RAW: // 35
          {
            mavlink_msg_rc_channels_raw_decode(&msg, &channels);
          }
          break;

        /*
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
        */

        case MAVLINK_MSG_ID_HEARTBEAT: // 27
          {

            mavlink_msg_heartbeat_decode(&msg, &heart);
            if (bitRead(heart.base_mode, 7)) {
              hbmotor_armed = 1;
            } else {
              hbmotor_armed = 0;
            }

            if (heart.system_status == 1) {
              hbstatus = "Booting up,";
            } else if (heart.system_status == 2) {
              hbstatus = "Calibrating,";
            } else if (heart.system_status == 3) {
              hbstatus = "Standby,";
            } else if (heart.system_status == 4) {
              hbstatus = "Active,";
            } else {
              hbstatus = "Other,";
            }
            //Serial.println(motor_armed);
            //Serial.println(system_status);

          }

        case MAVLINK_MSG_ID_VFR_HUD: // 74
          {
            mavlink_msg_vfr_hud_decode(&msg, &vrf);
          }
          break;

        case MAVLINK_MSG_ID_COMMAND_ACK: // 77
          {
            mavlink_command_ack_t packet;
            mavlink_msg_command_ack_decode(&msg, &packet);
            Serial.println("--MAVLINK_MSG_ID_COMMAND_ACK--");
            switch (packet.result) {
              case 0:
                {
                  Serial.print("Result: MAV_RESULT_ACCEPTED ");
                }
                break;
              case 1:
                {
                  Serial.print("Result: MAV_RESULT_TEMPORARILY_REJECTED ");
                }
                break;
              case 2:
                {
                  Serial.print("Result: MAV_RESULT_DENIED ");
                }
                break;
              case 3:
                {
                  Serial.print("Result: MAV_RESULT_UNSUPPORTED ");
                }
                break;
              case 4:
                {
                  Serial.print("Result: MAV_RESULT_FAILED ");
                }
                break;
              case 5:
                {
                  Serial.print("Result: MAV_RESULT_IN_PROGRESS ");
                }
                break;
              default:
                break;
            }
          }
          break;

        case MAVLINK_MSG_ID_STATUSTEXT: // 253
          {
            mavlink_msg_statustext_decode(&msg, &statustext);
            //String txt = "";
            //txt = String(statustext.text);
            //Serial.println(txt);
          }
          break;

        default:
          /*
            Serial.print("\n[ID: ");
            Serial.print(msg.msgid);
            //Serial.print("], [seq: ");
            //Serial.print(msg.seq);
            Serial.print("]");
          */
          break;
      }
    }
  }
}

void requestDatastream() {
  //Request Data from Pixhawk
  const int maxStreams = 7;

  uint8_t _req_stream_id[maxStreams] = {
    MAV_DATA_STREAM_RAW_SENSORS,
    MAV_DATA_STREAM_EXTENDED_STATUS,
    MAV_DATA_STREAM_RC_CHANNELS,
    MAV_DATA_STREAM_POSITION,
    MAV_DATA_STREAM_EXTRA1,
    MAV_DATA_STREAM_EXTRA2,
    MAV_DATA_STREAM_RAW_CONTROLLER
  };
  uint16_t _req_message_rate[maxStreams] = {
    0x02,
    0x02,
    0x05,
    0x02,
    0x05,
    0x02,
    0x02
  }; //number of times per second to request the data in hex
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

  for (int i = 0; i < maxStreams; i++) {
    // Initialize the required buffers
    mavlink_message_t msg;
    uint8_t buf[MAVLINK_MAX_PACKET_LEN];

    // Pack the message
    mavlink_msg_request_data_stream_pack(_system_id, _component_id, &msg, _target_system, _target_component, _req_stream_id[i], _req_message_rate[i], _start_stop);
    uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);  // Send the message (.write sends as bytes)

    _MavLinkSerial.write(buf, len); //Write data to serial port
  }
}

/* for future
  void sendCommandLong() {

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
  sendCommandLong();
  }

  void disarm() {
  CMD_LONG_param1 = 0;
  CMD_LONG_command = MAV_CMD_COMPONENT_ARM_DISARM;
  sendCommandLong();
  }
*/

void printInfo() {
  String all = "";
  String sChannels = String(channels.chan1_raw) + "," + String(channels.chan2_raw) + "," + String(channels.chan3_raw) + "," + String(channels.chan4_raw) + "," + String(channels.chan5_raw) + "," + String(channels.chan6_raw) + "," + String(channels.chan7_raw) + "," + String(channels.chan8_raw) + ";";
  String sVrf = String(vrf.alt) + "," + String(vrf.groundspeed) + "," + String(vrf.airspeed) + ";";
  String sAtti = String(attitude.pitch) + "," + String(attitude.roll) + ";";
  String sGps = String(gps.lat * 0.0000001, 10) + "," + String(gps.lon * 0.0000001, 10) + "," + String(gps.fix_type) + "," + String(gps.satellites_visible) + ";";
  String sSys = String(sys_status.voltage_battery) + "," + String(sys_status.current_battery) + "," + String(sys_status.battery_remaining) + ";";
  all = sChannels + sVrf + sAtti + sGps + sSys + hbstatus + String(hbmotor_armed) + ";" + statustext.text + "\n";
  Serial.print(all);
  all = "";
  sChannels = "";
  sVrf = "";
  sAtti = "";
  sGps = "";
  sSys = "";

}

// check if channels changed with a threshold of 10
void rcMiddleware(int xl, int yl, int xr, int yr) {
  int threshold = 5;
  int channels[4] = {xl, yl, xr, yr};
  for (int i = 0; i < 4; i++) {
    if (abs((channels[i] - oldChannels[i])) > threshold) {
      if (i == 0) {
        mav_override_rc(oldChannels[2], oldChannels[3], channels[0], oldChannels[1]);
      }
      else if (i == 1) {
        mav_override_rc(oldChannels[2], oldChannels[3], oldChannels[0], channels[1]);
      }
      else if (i == 2) {
        mav_override_rc(channels[2], oldChannels[3], oldChannels[0], oldChannels[1]);
      }
      else if (i == 3) {
        mav_override_rc(oldChannels[2], channels[3], oldChannels[0], oldChannels[1]);
      }
      else {};
      //mav_override_rc(channels[2], channels[3], channels[0], channels[1]);
      oldChannels[i] = channels[i];
    }
  }
}

void readJoystick() {
  unsigned long xl = 0;
  unsigned long yl = 0;
  unsigned long xr = 0;
  unsigned long yr = 0;

  for (int i = 0; i < 100; i++) {
    xl = xl + map(analogRead(xLeft), 0, 1023, 1000, 2000);
    yl = yl + map(analogRead(yLeft), 0, 1023, 1000, 2000);
    xr = xr + map(analogRead(xRight), 0, 1023, 1000, 2000);
    yr = yr + map(analogRead(yRight), 0, 1023, 1000, 2000);
  }
  xl = xl / 100;
  yl = yl / 100;
  xr = xr / 100;
  yr = yr / 100;

  rcMiddleware(xl, yl, xr, yr);

  //Serial.print("\nNew: "); Serial.print(xl); Serial.print(", "); Serial.print(yl); Serial.print(", "); Serial.print(xr); Serial.print(", "); Serial.print(yr); Serial.print("\n");
  //Serial.print("Old: "); Serial.print(oldChannels[0]); Serial.print(", "); Serial.print(oldChannels[1]); Serial.print(", "); Serial.print(oldChannels[2]); Serial.print(", "); Serial.print(oldChannels[3]); Serial.print("\n");

  /*
    channels[3] = yl;
    channels[2] = xl;
    channels[1] = yr;
    channels[0] = xr;
  */

  //
  //Serial.print("Left: "); Serial.print(xl); Serial.print("\t-\t"); Serial.print(yl);
  //Serial.print("\tRight: "); Serial.print(xr); Serial.print("\t-\t"); Serial.println(yr);
}

void mav_override_rc(int roll, int pitch, int throttle, int yaw) {
  mavlink_message_t msg;
  uint8_t buf[MAVLINK_MAX_PACKET_LEN];

  mavlink_msg_rc_channels_override_pack(0xFF, 0xBE, &msg, 1, 1, roll, pitch, throttle, yaw, 0, 0, 0, 0);
  uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);
  _MavLinkSerial.write(buf, len);
}
