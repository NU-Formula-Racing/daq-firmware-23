#include <Arduino.h>
#include "daqTemp.h"
#include "virtualTimer.h"

#define SERIAL_DEBUG

#if defined(ARDUINO_TEENSY40) || defined(ARDUINO_TEENSY41)
#include "teensy_can.h"
TeensyCAN<1> can_bus{};
#endif

#ifdef ARDUINO_ARCH_ESP32
#include "esp_can.h"
// TX pin and RX pin as input.
ESPCAN can_bus{};
#endif

// Initialize board
TempBoard temp_board;

// Structure for handling timers
VirtualTimerGroup read_timer;

// Set up for ambient_temp_signal.
CANSignal<float, 0, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(-40), false> ambient_temp_rx_signal{};
// Transmit motor_temp_signal and coolant_temp_signal.
CANRXMessage<1> rx_message{can_bus, temp_board.kCANId, ambient_temp_rx_signal};

// Read ambient temp sensor and convert to temperature units in Celsius.
// void ReadAmbientTempSensor()
// {
//   ambient_temp_signal = temp_board.ReadAmbientTempSensor();
// }

void receiveTemp()
{
  can_bus.Tick();
  Serial.print("Received: ");
  Serial.print("Temperature = ");
  Serial.print(float(ambient_temp_rx_signal));
  Serial.print(" degrees Celsius \n");
}

void setup()
{

#ifdef SERIAL_DEBUG
  // Initialize serial output
  Serial.begin(9600);
#endif

  // Initialize CAN bus.
  can_bus.Initialize(ICAN::BaudRate::kBaud1M);

  // Initialize our timer(s)
  read_timer.AddTimer(100, receiveTemp);
}

void loop()
{
  read_timer.Tick(millis());
}
