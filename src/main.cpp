#include <Arduino.h>
#include "daqBrake.h"
#include "virtualTimer.h"
#include <string>
#include <stdexcept>

#define SERIAL_DEBUG

// Choose what platform is going to be used for CAN data
// tranmission.
#if defined(ARDUINO_TEENSY40) || defined(ARDUINO_TEENSY41)
#include "teensy_can.h"
// The bus number is a template argument for Teensy: TeensyCAN<bus_num>
TeensyCAN<1> can_bus{};
#endif

#ifdef ARDUINO_ARCH_ESP32
#include "esp_can.h"
// The tx and rx pins are constructor arguments to ESPCan, which default to TX = 5, RX = 4
ESPCAN can_bus{};
#endif

// Initialize board.
BrakeBoard brake_board;

// Transmit over the correct CAN address based on what sensor value is being read.
// "front" = front press sensor
// "back" = rear press sensor
const uint16_t GetCANAddress(std::string sensor)
{
  if (sensor == "front")
  {
    return brake_board.kCANFRONT;
  }
  else if (sensor == "back")
  {
    return brake_board.kCANREAR;
  }
  else
  {
    throw std::invalid_argument("Invalid sensor, please input 'front' for front pressure sensor and 'back' for rear pressure sensor. ");
  }
}

// Structure for handling timers
VirtualTimerGroup read_timer;

CANSignal<float, 0, 16, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), false> brake_press_signal{};
// GetCANAddress should be passed the correct value:
// "front" = front press sensor
// "back" = rear press sensor
CANTXMessage<1> tx_message{can_bus, GetCANAddress("front"), 2, 10, read_timer, brake_press_signal};

void ReadBrakePressSensor()
{
  brake_press_signal = brake_board.ReadBrakePressSensor();
}

void setup()
{

#ifdef SERIAL_DEBUG
  // Initialize serial output
  Serial.begin(115200);
#endif

  // Initialize CAN bus.
  can_bus.Initialize(ICAN::BaudRate::kBaud1M);

  // Initialize our timer(s)
  read_timer.AddTimer(100, ReadBrakePressSensor);
}

void loop()
{
  read_timer.Tick(millis());

  // Does not work currently but will work when the CAN
  // library is fixed.
  // tx_message.Tick(millis());
}
