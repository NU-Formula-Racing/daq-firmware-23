#include <Arduino.h>
#include <daqBrakePress.h>

/**
 * @brief Construct a new Brake Board:: Brake Board object.
 * 
 */
BrakePressBoard::BrakePressBoard(){};

/**
 * @brief Reads the brake press sensor ADC value and converts it to pressure units (PSI).
 *
 * @return float
 */
float BrakePressBoard::ReadBrakePressSensor()
{
    uint16_t raw_ADC_value = 0;
    raw_ADC_value = analogRead(brakePressSensorPin);
    return (float)raw_ADC_value * brakePressScalar;
}
