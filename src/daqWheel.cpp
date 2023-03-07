#include <Arduino.h>
#include <daqWheel.h>

// #include <iostream>
// #include <sstream>
// #include <stdexcept>
#include <string>
// using namespace std;

/**
 * @brief Construct a new Wheel Board:: Wheel Board object
 *
 * @param can_frame
 */
WheelBoard::WheelBoard()
{
    current_pulse_time = 0;
    previous_pulse_time = 0;
    pulse_duration = 0;
};

/**
 * @brief Runs on interrupt to record pulse duration
 *
 */
void WheelBoard::ReadWheelSpeedSensorDuration()
{
    current_pulse_time = micros();
    pulse_duration = current_pulse_time - previous_pulse_time;
    previous_pulse_time = current_pulse_time;
};

/**
 * @brief Uses the pulse duration to calculate the wheel speed in MPH
 *
 * @return float
 */
float WheelBoard::ReadWheelSpeedSensor(const std::string& location)
{
    // Wheel Constants

    // Diameter of front and back wheels different.
    float diameter = 0;
    if (location == "front")
    {
        diameter = 0.88593;
    }
    else if (location == "back")
    {
        diameter = 2.85436;
    }
    else
    {
        // throw std::invalid_argument("Must use \"front\" or \"back\".");
        Serial.print("Must use \"front\" or \"back\".");
    }

    // Dependent on wheel diameter.
    const float kWheelCircumference = diameter * 3.141592;  // wheel circumference in m
    const float kMPStoMPH = 2.24;                           // m/s to MPH constant
    const int kMagnetCount = 22;
    // We multiply by 1M because our pulseDuration is measured in uS
    float pulse_frequency = 1000000.0 / (float)pulse_duration;
    float wheel_frequency = pulse_frequency / (float)kMagnetCount;
    return wheel_frequency * kWheelCircumference * kMPStoMPH;
};

/**
 * @brief Reads the brake temperature sensor ADC value and converts it to temperature units (degrees C)
 *
 * @return float
 */
float WheelBoard::ReadBrakeTempSensor()
{
    uint16_t raw_ADC_value = 0;
    raw_ADC_value = analogRead(brakeTempSensorPin);
    return (float)raw_ADC_value * brakeTempScalar + brakeTempOffset;
}

// enum class CANFrameAddress
// {
//     FL_CAN_FRAME_ADDRESS = 0x400,
//     FR_CAN_FRAME_ADDRESS = 0x401,
//     BL_CAN_FRAME_ADDRESS = 0x402,
//     BR_CAN_FRAME_ADDRESS = 0x403
// };