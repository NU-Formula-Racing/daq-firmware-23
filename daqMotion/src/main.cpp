#include <Arduino.h>
#include "daqWheel.h"

#define SERIAL_DEBUG

#if defined(ARDUINO_TEENSY40) || defined(ARDUINO_TEENSY41)
#include "teensy_can.h"
// The bus number is a template argument for Teensy: TeensyCAN<bus_num>
TeensyCAN<1> can_bus{};
#endif

#ifdef ARDUINO_ARCH_ESP32
#include "esp_can.h"
// The tx and rx pins are constructor arguments to ESPCan, which default to TX = 5, RX = 4
ESPCan can_bus{};
#endif

// Initialize board
CANFrameAddress can_frame_address = FL_CAN_FRAME_ADDRESS;
WheelBoard wheel_board;

//Structure for handling timers
virtualTimerGroup_S read_timer;

// TX CAN Message
CANSignal<float, 0, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0), false> wheel_speed_signal{}; 
CANSignal<float, 16, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(-40), false> brake_temp_signal{}; 
CANTXMessage<3> tx_message{can_bus, can_frame_address, 4, std::chrono::milliseconds{100}, wheel_speed_signal, brake_temp_signal};


void ReadWheelSpeedSensor() {
	wheel_speed_signal = wheel_board.ReadWheelSpeedSensor();
}

void ReadBrakeTempSensor() {
	brake_temp_signal = wheel_board.ReadBrakeTempSensor();
}

void IRAM_ATTR WheelSpeedISR() {
	wheel_board.ReadWheelSpeedSensorDuration();
}

void setup() {

  #ifdef SERIAL_DEBUG
  //Initialize serial output 
  Serial.begin(115200);
  #endif

  //This only works on ESP32, will crash on compile for Teensy
  //This makes us trigger reading wheel speed in an interrupt 
  attachInterrupt(wheel_board.wheelSpeedSensorPin, WheelSpeedISR, RISING); 

  // Initialize can bus
  can_bus.Initialize(ICAN::BaudRate::kBaud1M);

  //Initialize our timer(s)
  read_timer.addTimer(100, ReadWheelSpeedSensor);
  read_timer.addTimer(100, ReadBrakeTempSensor);
}

void loop() {
  read_timer.tick(millis());
  tx_message.Tick(millis());
}

