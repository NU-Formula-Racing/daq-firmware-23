#include <Arduino.h>
#include <daqMotion.h>
/**
 * @brief Construct a new Wheel Board:: Wheel Board object
 * 
 * @param can_frame 
 */
MotionBoard::MotionBoard()
{
    current_pulse_time = 0;
    previous_pulse_time = 0;
    pulse_duration = 0;
};

/**
 */
float MotionBoard::ReadGPS()
{
    float hello = 10; 
    return hello;
};
float MotionBoard::ReadAmbientTemp()
{
    float hello = 10; 
    return hello;
};
float MotionBoard::ReadAccel()
{
    float hello = 10; 
    return hello;
};