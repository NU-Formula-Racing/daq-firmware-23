#include <Arduino.h>
#include <daqTemp.h>

/**
 * @brief Constructor to setup pins and other configurations.
 */
TempBoard::TempBoard()
{
    // Enable interrupts.
    sei();
    // Specify both pins as an input.
    pinMode(temp_pin, INPUT);
    pinMode(flow_pin, INPUT);
}

/**
 * @brief Reads the ambient temperature sensor ADC value and converts it to temperature units (degrees C).
 *
 * @return float
 */
float TempBoard::ReadAmbientTempSensor()
{
    uint16_t raw_ADC_value = 0;
    raw_ADC_value = analogRead(temp_pin);
    // Serial.printf("ADC Value: %d\n", raw_ADC_value);
    float Vout = ((float)raw_ADC_value * VCC) / adc_resolution;
    float Rth = (VCC * R2 / Vout) - R2;
    float temperature = (1 / (A + (B * log(Rth) + (C * pow(log(Rth), 3)))));
    return temperature - 273.15;
}

/**
 * @brief Uses the flow count (amount of rising edges) and the change in time to calculate the flow rate (L/Hour).
 *
 * @return float
 */
float TempBoard::ReadCoolantFlowRate()
{
    int dtInSec = 2;
    float flowRate = flowCount * dtInSec / 7.5;
    return flowRate;
}