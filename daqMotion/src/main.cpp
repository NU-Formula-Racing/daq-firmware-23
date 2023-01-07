#include <Arduino.h>
#include "daqMotion.h"
#include "virtualTimer.h"

#define SERIAL_DEBUG

#ifdef ARDUINO_ARCH_ESP32
#include "esp_can.h"
// The tx and rx pins are constructor arguments to ESPCan, which default to TX = 5, RX = 4
ESPCAN can_bus{};
#endif

const int kGPS_CAN = 0x430;
const int kACCEL_CAN = 0x431;
const int kGYRO_CAN = 0x432;
// const int kGYRO_2_CAN = 0x432;
//factors and offsetsx`
const float imuFactor = 0.0005;
const float imuOffset = 0;
const float gpsFactor =  1e-07; 

float gpsOffset = 0;

// Initialize board
MotionBoard motion_board; 
//Structure for handling timers
VirtualTimerGroup read_timer;

//CANSignal<SignalType,start_position, length, factor, offset, is_signed> 

CANSignal<float, 0, 32, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), true> lat_signal{}; 
CANSignal<float, 32, 32, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), true> lon_signal{}; 

CANSignal<float, 0, 16, CANTemplateConvertFloat(0.0005), CANTemplateConvertFloat(0), true> accel_x{}; 
CANSignal<float, 16, 16, CANTemplateConvertFloat(0.0005), CANTemplateConvertFloat(0), true> accel_y{}; 
CANSignal<float, 32, 16, CANTemplateConvertFloat(0.0005), CANTemplateConvertFloat(0), true> accel_z{}; 

CANSignal<float, 0, 16, CANTemplateConvertFloat(0.0005), CANTemplateConvertFloat(0), true> gyro_x{}; 
CANSignal<float, 16, 16, CANTemplateConvertFloat(0.0005), CANTemplateConvertFloat(0), true> gyro_y{}; 
CANSignal<float, 32, 16, CANTemplateConvertFloat(0.0005), CANTemplateConvertFloat(0), true> gyro_z{}; 


CANTXMessage<3> accel_message{can_bus, kACCEL_CAN, 6, 10, read_timer, accel_x, accel_y, accel_z}; 
CANTXMessage<3> gyro_message{can_bus, kGYRO_CAN, 6, 10, read_timer, gyro_x, gyro_y, gyro_z};
CANTXMessage<2> gps_messaage{can_bus, kGPS_CAN, 8, 10, read_timer, lat_signal, lon_signal}; 
//can transmit rate  check on sensor spread sheet 
void ReadGPS() {
	float* gps =  motion_board.ReadGPS();
  lat_signal = gps[0], lon_signal = gps[1];
  Serial.println();
  Serial.print("GPS: ");
  Serial.print(lat_signal);
  Serial.print(" ");
  Serial.print(lon_signal);
  Serial.print(" ");
  // Serial.print(gps[2], 10);
  // Serial.print(" ");
  // Serial.println();
}

void ReadAccel() {
  float* accel = motion_board.ReadAccel();
  //setting the signals
  accel_x = accel[0], accel_y = accel[1], accel_z = accel[2];
  Serial.print("Accel: ");
  Serial.print(accel[0]);
  Serial.print(" ");
  Serial.print(accel[1]);
  Serial.print(" ");
  Serial.print(accel[2]);
  Serial.print(" ");
  Serial.println();
}
void ReadGyro() {
  float* gyro = motion_board.ReadGyro();
  //setting the signals
  gyro_x = gyro[0], gyro_y = gyro[1], gyro_z = gyro[2];

}
void setup() {
  #ifdef SERIAL_DEBUG
  //Initialize serial output 
  Serial.begin(9600);
  #endif

  // Initialize can bus
  can_bus.Initialize(ICAN::BaudRate::kBaud1M);
  // initalize the IMU and GPS sensor
  motion_board.setupMotionBoard();
  //Initialize our timers
  read_timer.AddTimer(100, ReadGPS);
  read_timer.AddTimer(100, ReadAccel);
  read_timer.AddTimer(100,ReadGyro);
  //sampling rate

}

void loop() {
  read_timer.Tick(millis());
}

