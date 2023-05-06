/* main.cpp */

// * Main program for the temeprature board

#include <Arduino.h>
#include "virtualTimer.h"
#include "flowRateSensor.h"
#include "thermistorSensor.h"

#define SERIAL_DEBUG

const bool DEBUG = true;

#pragma region CAN Setup
// Decide which can buss to use based on hardware
#if defined(ARDUINO_TEENSY40) || defined(ARDUINO_TEENSY41)
#include "teensy_can.h"
TeensyCAN<1> can_bus{};
#endif
#ifdef ARDUINO_ARCH_ESP32
#include "esp_can.h"
// TX pin and RX pin as input.
ESPCAN can_bus{};
#endif

// CAN addresses
const uint16_t kCANId1{0x420};
const uint16_t kCANId2{0x421};

// Structure for handling timers
VirtualTimerGroup read_timer;

// * Setup for ambient_temp_signal
CANSignal<float, 32, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(-40), false> ambient_temp_signal{};
// Transmit ambient_temp_signal.
CANTXMessage<1> tx_message_1{can_bus, kCANId1, 6, 100, read_timer, ambient_temp_signal};

// * Setup for coolant_flow signal
CANSignal<float, 0, 16, CANTemplateConvertFloat(0.01), CANTemplateConvertFloat(0), false> coolant_flow_signal{};
// Transmit coolant_flow_signal.
CANTXMessage<1> tx_message_2{can_bus, kCANId2, 2, 500, read_timer, coolant_flow_signal};
#pragma endregion

#pragma region Sensor Setup

int AMBIENT_TEMP_PIN = 35;
int COOLANT_TEMP_PIN = 32;
int FLOW_SENSOR_PIN = A7; // For the old esp
float R2 = 10000;

ThermistorSensor ambientTempSensor(AMBIENT_TEMP_PIN, R2);
ThermistorSensor coolantTempSensor(COOLANT_TEMP_PIN, R2);
FlowRateSensor flowSensor(FLOW_SENSOR_PIN);

#pragma endregion

// * Reading functions -- used for updating signals
// Read coolant flow rate and print value.
void ReadCoolantFlowRate()
{
  // float flowRate = flowSensor.Read();
  float flowRate = 10;
  coolant_flow_signal = flowRate;
  if (DEBUG)
  {
    Serial.println("----- FLOW RATE SENSOR -----");
    flowSensor.Print();
  }
}

// Read ambient temp sensor and print temperature (Celsius).
void ReadAmbientTempSensor()
{
  float temperature = ambientTempSensor.Read();
  ambient_temp_signal = temperature;
  if (DEBUG)
  {
    Serial.println("----- AMBIENT TEMP SENSOR -----");
    ambientTempSensor.Print();
  }
}

void ReadCoolantTemp()
{
  float temperature = coolantTempSensor.Read();
  if (DEBUG)
  {
    Serial.println("----- COOLANT TEMP SENSOR -----");
    coolantTempSensor.Print();
  }
}

void setup()
{

  #ifdef SERIAL_DEBUG
  // Initialize serial output
  Serial.begin(9600);
  #endif

  // Change Hardware Configuration for Coolant Temp Sensor
  coolantTempSensor.SetHardwareConfig(3.3, 4095);
  coolantTempSensor.SetSteinhartHartCoefficents(0.0011351346170947529264324571354073, 
                                                0.00023199412368471942874629067969408,
                                                0.000000090776459129764462247531747698604);

  // Initialize CAN bus.
  can_bus.Initialize(ICAN::BaudRate::kBaud1M);
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

  // * Debug
  ReadCoolantTemp();
  delay(500);
}
