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
    int X = pulseIn(this->_sensorPin, HIGH);
    int Y = pulseIn(this->_sensorPin, LOW);
    int TIME = X + Y;
    float FREQUENCY = 1000000/TIME;
    float WATER = FREQUENCY/7.5;
    float LS = WATER/60;
    return LS;
}

//
// Prints the value of thhe temperature sensor
//
void FlowRateSensor::Print() const
{
    float flowRate = this->Read();
    Serial.printf("Flow Rate: %0.2f L/s\n", flowRate);
}