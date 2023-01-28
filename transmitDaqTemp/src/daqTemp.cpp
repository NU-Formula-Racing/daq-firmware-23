#include <Arduino.h>
#include <daqTemp.h>
#include <math.h>

/**
 * @brief Construct a new Temp Board:: Temp Board Object
 *
 * @param can_frame
 */

TempBoard::TempBoard(){};

/**
 * @brief Reads the ambient temperature sensor ADC value and converts it to temperature units (degrees C).
 *
 * @return float
 */
float TempBoard::ReadAmbientTempSensor()
{
    uint16_t raw_ADC_value = 0;
    raw_ADC_value = analogRead(temp_pin);
    float Vout = ((float)raw_ADC_value * VCC) / adc_resolution;
    float Rth = (VCC * R2 / Vout) - R2;
    float temperature = (1 / (A + (B * log(Rth) + (C * pow(log(Rth), 3)))));
    temperature = temperature - 273.15;
    return temperature;
}

float TempBoard::CoolantFlowRate()
{
    float dtInSec = 1000 / (float)delayTime;
    float flowRate = flowCount * dtInSec * 60 / 7.5;
    return flowRate;
}

void TempBoard::UpdateFlowCount()
{
    flowCount++;
}
