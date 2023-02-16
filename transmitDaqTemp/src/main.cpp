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
// Transmit ambient_temp_signal.
CANTXMessage<1> tx_message_1{can_bus, temp_board.kCANId1, 6, 100, read_timer, ambient_temp_signal};

// Coolant Flow Rate
CANSignal<float, 0, 16, CANTemplateConvertFloat(0.01), CANTemplateConvertFloat(0), false> coolant_flow_signal{};
// Transmit coolant_flow_signal.
CANTXMessage<1> tx_message_2{can_bus, temp_board.kCANId2, 2, 500, read_timer, coolant_flow_signal};

// Read coolant flow rate and print value.
void ReadCoolantFlowRate()
{
  float coolant_flow_rate = temp_board.ReadCoolantFlowRate();
  Serial.printf("Flow Count: %d\nFlow Rate : %.2f L/min\n", temp_board.flowCount, coolant_flow_rate / 60);
  // DBC has units in L/min.
  coolant_flow_signal = coolant_flow_rate / 60;
  // Reset flowCount.
  temp_board.flowCount = 0;
}

// Update flow count every rising edge (change from 0 to 1).
// This could not be implemented in the tempboard constructor.
void UpdateFlowCount()
{
  temp_board.flowCount++;
}

// Read ambient temp sensor and print temperature (Celsius).
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

  // Update flow count for every rising edge (0 to 1).
  // This could not be implemented in the tempboard constructor.
  attachInterrupt(temp_board.flow_pin, UpdateFlowCount, RISING);

  // Initialize our timer(s) to read each sensor every
  // specified amount of time (ms).
  read_timer.AddTimer(100, ReadAmbientTempSensor);
  // Delay is no longer necessary as PrintCoolantFlowRate is called
  // every 500 ms.
  read_timer.AddTimer(500, ReadCoolantFlowRate);
}

void loop()
{
  // Fix hardware bug (ESP shutdown pin getting voltage).
  pinMode(GPIO_NUM_15, OUTPUT);
  digitalWrite(GPIO_NUM_15, LOW);
  // can_bus.Tick();
  read_timer.Tick(millis());
}
