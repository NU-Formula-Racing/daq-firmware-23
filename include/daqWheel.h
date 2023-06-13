#include <string>
/**
 * @brief Definitions and constants for the DAQ wheel speed board. 
 *
 */
class WheelSpeedBoard
{
public:

    // Constructor 
    WheelSpeedBoard();

    // Functions
    void ReadWheelSpeedSensorDuration();
    float ReadWheelSpeedSensor(bool location);
    float ReadBrakeTempSensor();

    // Pin 
    static constexpr int wheelSpeedSensorPin = 34;
    

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