#include <Arduino.h>
#include <daqBrake.h>

/**
 * @brief Construct a new Break Press Board:: Break Press Board Object
 *
 * @param can_frame
 */

BrakeBoard::BrakeBoard(){};

/**
 * @brief Reads the brake press sensor ADC value and converts it to pressure units (PSI).
 *
 * @return float
 */
float BrakeBoard::ReadBrakePressSensor()
{
    uint16_t raw_ADC_value = 0;
    raw_ADC_value = analogRead(BrakePressSensorPin);
    return (float)raw_ADC_value * brakePressScalar;
}
