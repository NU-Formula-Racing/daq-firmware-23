/**
 * @brief Definitions and constants for the DAQ wheel board. Initializes with a wheel speed sensor pin, 
 * brake temp sensor pin, sus pos sensor pin, and can frame address.
 * 
 */
class WheelBoard
{
    public:
        static constexpr int wheelSpeedSensorPin = 1;
        WheelBoard();
        void ReadWheelSpeedSensorDuration();
        float ReadWheelSpeedSensor();
        float ReadBrakeTempSensor();

    private:
        // pins
        static constexpr int brakeTempSensorPin = 2;
        // scalars and offsets
        static constexpr float brakeTempScalar = 1;
        static constexpr float brakeTempOffset = 0;
        // wheel speed sensor
        unsigned long current_pulse_time;
        unsigned long previous_pulse_time;
        unsigned long pulse_duration;
};

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
float WheelBoard::ReadWheelSpeedSensor()
{
    // Wheel constants
    const float kWheelCircumference = 1.26; //wheel circumference in m
    const float kMPStoMPH = 2.24; // m/s to MPH constant 
    const int kMagnetCount = 20;
    //We multiply by 1M because our pulseDuration is measured in uS
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

enum CANFrameAddress
{
    FL_CAN_FRAME_ADDRESS = 0x400,
    FR_CAN_FRAME_ADDRESS = 0x401,
    BL_CAN_FRAME_ADDRESS = 0x402,
    BR_CAN_FRAME_ADDRESS = 0x403
};