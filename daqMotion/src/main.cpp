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
const int kGYRO_2_CAN = 0x432;
//factors and offsets
static constexpr float imuFactor = 0.0005;
static constexpr float imuOffset = 0;
static constexpr float gpsFactor =  1e-07;
static constexpr float gpsOffset = 0;

// Initialize board
MotionBoard motion_board; 
//Structure for handling timers
VirtualTimerGroup read_timer;

//CANSignal<SignalType,start_position, length, factor, offset, is_signed> 
//TODO: customize the template arguments
CANSignal<float, 0, 24, CANTemplateConvertFloat(gpsFactor), CANTemplateConvertFloat(0), true> lat_signal{}; 
CANSignal<float, 32, 24, CANTemplateConvertFloat(gpsFactor), CANTemplateConvertFloat(0), true> lon_signal{}; 

CANSignal<float, 0, 16, CANTemplateConvertFloat(imuFactor), CANTemplateConvertFloat(0), true> accel_x{}; 
CANSignal<float, 16, 32, CANTemplateConvertFloat(imuFactor), CANTemplateConvertFloat(0), true> accel_y{}; 
CANSignal<float, 32, 48, CANTemplateConvertFloat(imuFactor), CANTemplateConvertFloat(0), true> accel_z{}; 

CANSignal<float, 0, 16, CANTemplateConvertFloat(imuFactor), CANTemplateConvertFloat(0), true> gyro_x{}; 
CANSignal<float, 16, 32, CANTemplateConvertFloat(imuFactor), CANTemplateConvertFloat(0), true> gyro_y{}; 
CANSignal<float, 32, 48, CANTemplateConvertFloat(imuFactor), CANTemplateConvertFloat(0), true> gyro_z{}; 




CANTXMessage<3> tx_message{can_bus, kACCEL_CAN, 4, 100, read_timer, accel_x, accel_y, accel_z};
/* CANTXMessage<3> tx_message{can_bus, kGYRO_CAN, 4, 100, read_timer, gyro_x, gyro_y, gyro_z};
CANTXMessage<2> tx_message{can_bus, kGPS_CAN, 4, 100, read_timer, lon_signal, lat_signal}; */

void ReadGPS() {
	float* gps =  motion_board.ReadGPS();
  lat_signal = gps[0], lon_signal = gps[1];
}

void ReadAccel() {
  float* accel = motion_board.ReadAccel();
  //setting the signals
  accel_x = accel[0], accel_y = accel[1], accel_z = accel[2];
}
void ReadGyro() {
  float* gyro = motion_board.ReadGyro();
  //setting the signals
  gyro_x = gyro[0], gyro_y = gyro[1], gyro_z = gyro[2];

}
void setup() {
  #ifdef SERIAL_DEBUG
  //Initialize serial output 
  Serial.begin(115200);
  #endif

  // Initialize can bus
  can_bus.Initialize(ICAN::BaudRate::kBaud1M);

  // initalize the IMU and GPS sensor
  motion_board.setupMotionBoard();
  //Initialize our timer(s)
  read_timer.AddTimer(1000, ReadGPS);
  read_timer.AddTimer(100, ReadAccel);

}

void loop() {
  read_timer.Tick(millis());
}

