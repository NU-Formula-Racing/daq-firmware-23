/* flowRateSensor.cpp */

#include <Arduino.h>
#include "flowRateSensor.h"

// Implementation of the FlowRateSensor class

//
// Reads the flow rate from the sensor, and returns it in liters/second
// Note, this function will halt the execution of the program until a pulse from the sensor is read
// Normally, this delay is insignificant, but it should be noted.
//
float FlowRateSensor::Read() const
{
    int x = pulseIn(this->_sensorPin, HIGH);
    int y = pulseIn(this->_sensorPin, LOW);
    int time = x + y;
    if (time == 0) return 0; // Handle timeout
    float freq = 1000000/time;
    float water = freq/7.5;
    float litersPerSec = water/60;
    return litersPerSec;
}

//
// Prints the value of thhe temperature sensor
//
void FlowRateSensor::Print() const
{
    float flowRate = this->Read();
    Serial.printf("Flow Rate: %0.2f L/s\n", flowRate);
}