#include <string>
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
    float ReadWheelSpeedSensor(const std::string & location);
    float ReadBrakeTempSensor();
    enum CANFrameAddress
    {
        FL_CAN_FRAME_ADDRESS = 0x400,
        FR_CAN_FRAME_ADDRESS = 0x401,
        BL_CAN_FRAME_ADDRESS = 0x402,
        BR_CAN_FRAME_ADDRESS = 0x403
    };

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