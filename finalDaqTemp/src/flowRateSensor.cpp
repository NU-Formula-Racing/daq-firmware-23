/* flowRateSensor.cpp */

#include <Arduino.h>
#include "flowRateSensor.h"

// Implementation of the FlowRateSensor class

void FlowRateSensor::HandleInterrupt()
{
    this->_flowCount++;
}
//
// Reads the flow rate from the sensor, and returns it in liters/second
// Note, this function will halt the execution of the program until a pulse from the sensor is read
// Normally, this delay is insignificant, but it should be noted.
//
float FlowRateSensor::Read()
{
    /*
    int x = pulseIn(this->_sensorPin, HIGH);
    int y = pulseIn(this->_sensorPin, LOW);
    int time = x + y;
    if (time == 0) return 0; // Handle timeout
    float freq = 1000000/time;
    float water = freq/7.5;
    float litersPerSec = water/60;
    return litersPerSec;
    */

   unsigned long readTime = millis();
   // Serial.printf("reading flow rate sensor at %d millis\n", readTime);
   float dtInSec = (readTime - this->_lastReadTime) / (float)1000;
   Serial.printf("flow count: %d\n", this->_flowCount);
   Serial.printf("dtInSec: %f\n ", dtInSec);
   float flowRate = this->_flowCount * dtInSec / 7.5;
   this->_flowCount = 0;
   this->_lastReadTime = readTime;
   return flowRate;
}

//
// Prints the value of thhe temperature sensor
//
void FlowRateSensor::Print()
{
    float flowRate = this->Read();
    Serial.printf("Flow Rate: %0.2f L/s\n", flowRate);
}