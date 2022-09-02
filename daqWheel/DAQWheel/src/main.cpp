#include <Arduino.h>
#include "pinDefs.h"

#define SERIAL_DEBUG

//Structure for handling timers 
typedef struct 
{
  unsigned long lastTick = 0;
  unsigned long interval = 1000;

  void init(unsigned long newInterval)
  {
    interval = newInterval;
    lastTick = millis();
  }

  bool tick()
  {
    if(lastTick + interval <= millis())
    {
      lastTick = millis();
      return true;
    }
    else
    {
      return false;
    }
  }

} task_timer_t;

task_timer_t t100;

//Structure for handling wheel speed sensor values 
typedef struct 
{
  unsigned long currentPulseTime = 0;
  unsigned long previousPulseTime = 0;
  unsigned long pulseDuration = 0;
  const int pin = WHEEL_SPEED_SENSOR_PIN;

  void readSensor()
  {
    currentPulseTime = micros();
    pulseDuration = currentPulseTime - previousPulseTime;
    previousPulseTime = currentPulseTime; 
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
  t100.init(100);

}

void loop() {

  if(t100.tick())
  {
    //Read Suspension Position
    suspensionPositionSensor.readSensor();

    //Read Brake Temperature
    brakeTemperatureSensor.readSensor();

    //Read Wheel Speed
    wheelSpeedSensor.readSensor();

    //Populate sensor values to CAN frame + transmit 

  }
}

