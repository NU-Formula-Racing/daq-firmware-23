#include <Arduino.h>
#include "virtualTimer.h"
#include "daqBrakePress.h"


#define SERIAL_DEBUG

// Choose the front or back brake pressure sensor. 
// true: front brake pressure
// false: back brake pressure
#define isFront (true)

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
BrakePressBoard brake_board;

// Structure for handling timers
VirtualTimerGroup read_timer;


// Set up CAN messages.
CANSignal<float, 0, 16, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), false> front_brake_press_signal{};
CANSignal<float, 16, 16, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), false> back_brake_press_signal{};
CANTXMessage<2> BRAKE_PRESS_TX_MESSAGE{can_bus, brake_board.CANAddress, 4, 100, read_timer, front_brake_press_signal, back_brake_press_signal};


/**
 * @brief Read and record CAN value depending on 
 * whether the current sensor is the front or back 
 * brake pressure sensor.
 * 
 */
void ReadBrakePressSensor()
{
// Front Brake Pressure: 
#if isFront
  front_brake_press_signal = brake_board.ReadBrakePressSensor();
  Serial.printf("Front Brake Pressure: %f \n", front_brake_press_signal);
// Back Brake Pressure:
#else 
  back_brake_press_signal = brake_board.ReadBrakePressSensor();
  Serial.printf("Back Brake Pressure: %f \n", back_brake_press_signal);
#endif
  
}

void setup()
{

#ifdef SERIAL_DEBUG
  // Initialize serial output
  Serial.begin(9600);
#endif

  // Initialize CAN bus.
  can_bus.Initialize(ICAN::BaudRate::kBaud1M);

  // Initialize our timer.
  read_timer.AddTimer(100, ReadBrakePressSensor);
}

void loop()
{
  read_timer.Tick(millis());
}
