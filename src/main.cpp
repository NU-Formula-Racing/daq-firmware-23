/* main.cpp */

// * Main program for the temeprature board

#include <Arduino.h>
#include "virtualTimer.h"
#include "flowRateSensor.h"
#include "thermistorSensor.h"

#define SERIAL_DEBUG

const bool PRINT_SENSOR_DATA = true;

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
const uint16_t pTrainAddr{0x420};
const uint16_t coolantAddr{0x421};

// Structure for handling timers
VirtualTimerGroup readTimer;

// * Setup for ambient_temp_signal
CANSignal<float, 32, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(-40), false> ambientTempSignal{};
// * Setup for coolant_temp_signal
CANSignal<float, 16, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(-40), false> coolant_temp_signal{};

// * Transmit pTrain signals
CANTXMessage<2> pTrainMessage{can_bus, pTrainAddr, 6, 100, readTimer, ambientTempSignal, coolant_temp_signal};

// * Setup for coolant_flow signal
CANSignal<float, 0, 16, CANTemplateConvertFloat(0.01), CANTemplateConvertFloat(0), false> coolantFlowSignal{};
// Transmit coolant signals
CANTXMessage<1> coolantMessage{can_bus, coolantAddr, 2, 500, readTimer, coolantFlowSignal};
#pragma endregion

#pragma region Sensor Setup

int AMBIENT_TEMP_PIN = 35;
int COOLANT_TEMP_PIN = 32;
int FLOW_SENSOR_PIN = 34; // For the old esp
float R2 = 10000;

ThermistorSensor ambientTempSensor(AMBIENT_TEMP_PIN, R2);
ThermistorSensor coolantTempSensor(COOLANT_TEMP_PIN, R2);
FlowRateSensor flowSensor(FLOW_SENSOR_PIN);

#pragma endregion

#pragma region Reading functions
// * Reading functions -- used for updating signals
// Read coolant flow rate and print value.
void ReadCoolantFlowRate()
{
  float flowRate = flowSensor.Read();
  coolantFlowSignal = flowRate;
  if (PRINT_SENSOR_DATA)
  {
    Serial.println("----- FLOW RATE SENSOR -----");
    flowSensor.Print();
  }
}

// Read ambient temp sensor and print temperature (Celsius).
void ReadAmbientTemp()
{
  float temperature = ambientTempSensor.Read();
  ambientTempSignal = temperature;
  if (PRINT_SENSOR_DATA)
  {
    Serial.println("----- AMBIENT TEMP SENSOR -----");
    ambientTempSensor.Print();
  }
}

void ReadCoolantTemp()
{
  float temperature = coolantTempSensor.Read();
  coolant_temp_signal = temperature;
  if (PRINT_SENSOR_DATA)
  {
    Serial.println("----- COOLANT TEMP SENSOR -----");
    coolantTempSensor.Print();
  }
}

#pragma endregion

// Ugly workaround
void UpdateFlowRate()
{
  // Serial.printf("Updating flow rate \n");
  flowSensor.HandleInterrupt();
}

void setup()
{

#ifdef SERIAL_DEBUG
  // Initialize serial output
  Serial.begin(9600);
#endif

  // Attach interrupts
  attachInterrupt(FLOW_SENSOR_PIN, UpdateFlowRate, RISING);

  // Change Hardware Configuration for Coolant Temp Sensor
  coolantTempSensor.SetSteinhartHartCoefficents(0.0011351346170947529264324571354073,
                                                0.00023199412368471942874629067969408,
                                                0.000000090776459129764462247531747698604);

  // Initialize CAN bus.
  can_bus.Initialize(ICAN::BaudRate::kBaud1M);

  // Initialize our timer(s) to read each sensor every
  // specified amount of time (ms).
  readTimer.AddTimer(100, ReadAmbientTemp);
  readTimer.AddTimer(100, ReadCoolantTemp);
  // Delay is no longer necessary as PrintCoolantFlowRate is called
  // every 500 ms.
  readTimer.AddTimer(500, ReadCoolantFlowRate);
}

void loop()
{
  // Fix hardware bug (ESP shutdown pin getting voltage).
  pinMode(GPIO_NUM_15, OUTPUT);
  digitalWrite(GPIO_NUM_15, LOW);
  // can_bus.Tick();
  readTimer.Tick(millis());
}
