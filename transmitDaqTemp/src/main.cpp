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
CANSignal<float, 32, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(-40), false> ambient_temp_signal{};
// Transmit motor_temp_signal and coolant_temp_signal.
CANTXMessage<1> tx_message{can_bus, temp_board.kCANId, 6, 100, read_timer, ambient_temp_signal};

// Calculate coolant flow rate.
void PrintCoolantFlowRate()
{
  float coolant_flow_rate = temp_board.CoolantFlowRate();
  Serial.printf("Flow Count: %d\nFlow Rate : %.2f L/hour\n", temp_board.flowCount, coolant_flow_rate);
  // Reset flowCount
  temp_board.flowCount = 0;
}

// Read ambient temp sensor and convert to temperature units in Celsius.
void ReadAmbientTempSensor()
{
  ambient_temp_signal = temp_board.ReadAmbientTempSensor();
  Serial.print("Temperature = ");
  Serial.print(ambient_temp_signal);
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
  read_timer.AddTimer(100, ReadAmbientTempSensor);
  // read_timer.AddTimer(200, PrintCoolantFlowRate);
}

void loop()
{
  delay(0);
  // delay(temp_board.delayTime);
  read_timer.Tick(millis());
  // PrintCoolantFlowRate();

  // Does not work currently but will work when the CAN
  // library is fixed.
  // tx_message.Tick(millis());
}
