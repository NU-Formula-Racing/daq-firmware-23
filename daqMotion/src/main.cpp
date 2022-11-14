#include <Arduino.h>
#include "daqMotion.h"
#include "virtualTimer.h"

#define SERIAL_DEBUG

#ifdef ARDUINO_ARCH_ESP32
#include "esp_can.h"
// The tx and rx pins are constructor arguments to ESPCan, which default to TX = 5, RX = 4
ESPCAN can_bus{};
#endif



// Initialize board
MotionBoard motion_board; 
//Structure for handling timers
VirtualTimerGroup read_timer;

// TX CAN Message 
//TODO: customize this for MotionBoard
CANSignal<float, 0, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0), false> wheel_speed_signal{}; 
CANSignal<float, 16, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(-40), false> brake_temp_signal{}; 
CANTXMessage<2> tx_message{can_bus, 0x400, 4, 100, read_timer, wheel_speed_signal, brake_temp_signal};


void ReadGPS() {
	wheel_speed_signal = motion_board.ReadGPS();
}

void ReadAccel() {
  wheel_speed_signal = motion_board.ReadAccel();
}

void setup() {

  #ifdef SERIAL_DEBUG
  //Initialize serial output 
  Serial.begin(115200);
  #endif

  // Initialize can bus
  can_bus.Initialize(ICAN::BaudRate::kBaud1M);

  //Initialize our timer(s)
  read_timer.AddTimer(100, ReadGPS);
  read_timer.AddTimer(100, ReadAccel);


// initalize the IMU and GPS sensor
if  (! motion_board.setupMotionBoard() ) {
  Serial.print("failed to properly set up the motionBoard");
  while(1);
}
}

void loop() {
  read_timer.Tick(millis());
}

