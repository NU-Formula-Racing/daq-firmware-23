#include <Arduino.h>
#include "pinDefs.h"
#include "../lib/CAN/include/virtualTimer.h"
#include "../lib/CAN/include/app_can.h"

#define SERIAL_DEBUG
#define ARDUINO_TEENSY40

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

//Structure for handling timers
virtualTimer_S tSuspot;
virtualTimer_S tWheelSpeed;
virtualTimer_S tBrakeTemp;
virtualTimerGroup_S readTimer;
virtualTimerGroup_S writeTimer;

// TX CAN Message
int messageId = 0x400;
CANSignal<uint16_t, 0, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), true> wheelSpeedSignal{}; 
CANSignal<uint16_t, 16, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), true> brakeTempSignal{}; 
CANSignal<uint16_t, 32, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), true> susPotSignal{}; 
CANTXMessage<3> tx_message{can_bus, messageId, 4, std::chrono::milliseconds{100}, wheelSpeedSignal, brakeTempSignal, susPotSignal};

//Structure for handling wheel speed sensor values 
typedef struct 
{
  unsigned long currentPulseTime = 0;
  unsigned long previousPulseTime = 0;
  unsigned long pulseDuration = 0;
  uint32_t wheelSpeed = 0;
  const int pin = WHEEL_SPEED_SENSOR_PIN;

  void readDuration()
  {
    currentPulseTime = micros();
    pulseDuration = currentPulseTime - previousPulseTime;
    previousPulseTime = currentPulseTime;
  }

  void readSensor()
  {
    // Wheel constants
    const float wheelCircumference = 1.26; //wheel circumference in m
    const float MPStoMPH = 2.24; // m/s to MPH constant 
    const int magnetCount = 20;
    //We multiply by 1M because our pulseDuration is measured in uS
    float pulseFrequency = 1000000.0 / (float)pulseDuration;
    float wheelFrequency = pulseFrequency / (float)magnetCount; 
    wheelSpeed = (uint32_t)(wheelFrequency * wheelCircumference * MPStoMPH);
  }

  void writeCAN()
  {
    wheelSpeedSignal = (uint16_t)wheelSpeed
  }

} wheel_speed_sensor_t;

wheel_speed_sensor_t wheelSpeedSensor;

//Structure for handling brake temperature values
typedef struct 
{
  uint32_t rawADCValue = 0;
  uint32_t value = 0;

  const uint32_t scalar = BRAKE_TEMPERATURE_SCALAR;
  const uint32_t offset = BRAKE_TEMPERATURE_OFFSET;

  const int pin = BRAKE_TEMPERATURE_SENSOR_PIN;

  void readSensor()
  {
    rawADCValue = analogRead(pin);
    value = rawADCValue * scalar + offset;
  }

  void writeCAN()
  {
    brakeTempSignal = (uint16_t)value;
  }

} brake_temperature_sensor_t;

brake_temperature_sensor_t brakeTemperatureSensor;

//Structure for handling suspension position values
typedef struct 
{
  uint32_t rawADCValue = 0;
  uint32_t value = 0;

  const uint32_t scalar = SUSPENSION_POSITION_SCALAR;
  const uint32_t offset = SUSPENSION_POSITION_OFFSET;

  const int pin = SUSPENSION_POSITION_SENSOR_PIN;

  void readSensor()
  {
    rawADCValue = analogRead(pin);
    value = rawADCValue * scalar + offset;
  }

  void writeCAN()
  {
    susPotSignal = (uint16_t)value;
  }

} suspension_position_sensor_t;

suspension_position_sensor_t suspensionPositionSensor;

//Interrupt function for handling wheel speed 
void IRAM_ATTR wheelSpeedISR() {
	wheelSpeedSensor.readDuration();
}

void setup() {

  #ifdef SERIAL_DEBUG
  //Initialize serial output 
  Serial.begin(115200);
  #endif

  //This only works on ESP32, will crash on compile for Teensy
  //This makes us trigger reading wheel speed in an interrupt 
  attachInterrupt(wheelSpeedSensor.pin, wheelSpeedISR, RISING); 

  //Initialize our timer(s)
  readTimer.addTimer(100, wheelSpeedSensor.readSensor);
  readTimer.addTimer(100, suspensionPositionSensor.readSensor);
  readTimer.addTimer(1000, brakeTemperatureSensor.readSensor);

  writeTimer.addTimer(100, wheelSpeedSensor.writeCAN);
  writeTimer.addTimer(100, suspensionPositionSensor.writeCAN);
  writeTimer.addTimer(1000, brakeTemperatureSensor.writeCAN);

  can_bus.Initialize(ICAN::BaudRate::kBaud1M);
}

void loop() {
  readTimer.tick(millis());
  writeTimer.tick(millis());
  tx_message.Tick(millis());
}

