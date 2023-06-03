/* flowRateSensor.cpp */

#include <Arduino.h>
#include "flowRateSensor.h"

// Implementation of the FlowRateSensor class

void FlowRateSensor::HandleInterrupt()
{
    this->_flowCount++;
}
//
// Reads the flow rate from the sensor, and returns it in liters/minute
//
float FlowRateSensor::Read()
{
    // Calculate dt
    unsigned long readTime = millis();
    // Serial.printf("reading flow rate sensor at %d millis\n", readTime);
    float dtInSec = (float)(readTime - this->_lastReadTime) / 1000;
    this->_lastReadTime = readTime;

    // Edge case
    if (dtInSec < 10e-5)
        return this->_lastRead;

    // Serial.printf("flow count: %d\n", this->_flowCount);
    // Serial.printf("dtInSec: %f\n ", dtInSec);

    // Calculate flow rate
    float flowRate = this->_flowCount / (dtInSec * 5.5);

    // Reset state
    this->_lastRead = flowRate;
    this->_flowCount = 0;

    return flowRate;
}

//
// Prints the value of thhe temperature sensor
//
void FlowRateSensor::Print()
{
    float flowRate = this->_lastRead;
    Serial.printf("Flow Rate: %0.2f L/min\n", flowRate);
}