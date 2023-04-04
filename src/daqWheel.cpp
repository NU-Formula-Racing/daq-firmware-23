#include <Arduino.h>
#include <daqWheel.h>

#include <cmath>
#include <string>

/**
 * @brief Construct a new Wheel Board:: Wheel Board object
 *
 * @param can_frame
 */
WheelBoard::WheelBoard()
{
    // Enable interrupts.
    sei();
    // Specify both pins as an input.
    pinMode(wheelSpeedSensorPin, INPUT);
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
        Serial.print("Must use \"front\" or \"back\" as input to ReadWheelSpeedSensor().\n");
    }

    // Dependent on wheel diameter.
    const float kWheelCircumference = diameter * 3.141592;  // wheel circumference in m
    const float kMPStoMPH = 2.24;                           // m/s to MPH constant
    const int kMagnetCount = 22;
    // We multiply by 1M because our pulseDuration is measured in uS
    float pulse_frequency = 1000000.0 / (float)pulse_duration;
    float wheel_frequency = pulse_frequency / (float)kMagnetCount;
    // Maximum possible pulse duration for 1 MPH, if the difference between the current time and the previous pulse
    // time is greater than this value then the wheel has stopped moving.
    float max_threshold = 1000000.0 / ((1 / (kWheelCircumference * kMPStoMPH)) * (float)kMagnetCount);
    // Display 0 MPH when not moving or when wheel speed has stopped moving.
    if (isinf(wheel_frequency) || (micros() - previous_pulse_time) >= max_threshold)
    {
        return 0;
    }
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