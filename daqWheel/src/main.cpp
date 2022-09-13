#include <Arduino.h>
#include "pinDefs.h"
#include "../lib/CAN/include/virtualTimer.h"
#include "../lib/CAN/include/app_can.h"

#define SERIAL_DEBUG

//Structure for handling timers
virtualTimer_S tSuspot;
virtualTimer_S tWheelSpeed;
virtualTimer_S tBrakeTemp;
virtualTimerGroup_S readTimer;
virtualTimerGroup_S writeTimer;

// TX CAN Message 
app_can_message_t tx_msg;

//Structure for handling wheel speed sensor values 
typedef struct 
{
  unsigned long currentPulseTime = 0;
  unsigned long previousPulseTime = 0;
  unsigned long pulseDuration = 0;
  uint32_t value = 0;
  const int pin = WHEEL_SPEED_SENSOR_PIN;

  void readSensor()
  {
    currentPulseTime = micros();
    pulseDuration = currentPulseTime - previousPulseTime;
    previousPulseTime = currentPulseTime; 
    value = 0; // calculate this using pulse info
  }

  void writeCAN()
  {
    // create can message and send??
    tx_msg.id = 0x400;
    tx_msg.len = 0x6;
    tx_msg.data = value;
    app_can_write(&tx_msg);
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
    // create can message and send??
    tx_msg.id = 0x400;
    tx_msg.len = 0x6;
    tx_msg.data = value;
    app_can_write(&tx_msg);
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
    // create can message and send??
    tx_msg.id = 0x400;
    tx_msg.len = 0x6;
    tx_msg.data = value;
    app_can_write(&tx_msg);
  }

} suspension_position_sensor_t;

suspension_position_sensor_t suspensionPositionSensor;

//Interrupt function for handling wheel speed 
void IRAM_ATTR wheelSpeedISR() {
	wheelSpeedSensor.readSensor();
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
  tWheelSpeed.init(1, wheelSpeedSensor.readSensor);
  tSuspot.init(100, suspensionPositionSensor.readSensor);
  tBrakeTemp.init(1000, brakeTemperatureSensor.readSensor);

  readTimer.addTimer(&tWheelSpeed);
  readTimer.addTimer(&tSuspot);
  readTimer.addTimer(&tBrakeTemp);

  writeTimer.addTimer(100, wheelSpeedSensor.writeCAN);
  writeTimer.addTimer(100, suspensionPositionSensor.writeCAN);
  writeTimer.addTimer(1000, brakeTemperatureSensor.writeCAN);
}

void loop() {
  readTimer.tick();
  writeTimer.tick();
}

