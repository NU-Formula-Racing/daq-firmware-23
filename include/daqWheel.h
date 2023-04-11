#include <string>
/**
 * @brief Definitions and constants for the DAQ wheel board. Initializes with a wheel speed sensor pin,
 * brake temp sensor pin, sus pos sensor pin, and can frame address.
 *
 */
class WheelBoard
{
public:
    WheelBoard();
    void ReadWheelSpeedSensorDuration();
    float ReadWheelSpeedSensor(const std::string& location);
    static constexpr int wheelSpeedSensorPin = 34;
    float ReadBrakeTempSensor();
    enum CANFrameAddress
    {
        FL_CAN_FRAME_ADDRESS = 0x400,
        FR_CAN_FRAME_ADDRESS = 0x401,
        BL_CAN_FRAME_ADDRESS = 0x402,
        BR_CAN_FRAME_ADDRESS = 0x403
    };

private:
    // Wheel Speed Sensor //
    unsigned long current_pulse_time;
    unsigned long previous_pulse_time;
    unsigned long pulse_duration;
    // Brake Temp Sensor //
    // Pin
    static constexpr int brakeTempSensorPin = 35;
    // Scalars and Offsets
    static constexpr float brakeTempScalar = 1;
    static constexpr float brakeTempOffset = 0;
};