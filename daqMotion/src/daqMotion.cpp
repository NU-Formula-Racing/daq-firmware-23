#include <Arduino.h>
#include <daqWheel.h>

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

};
float ReadAmbientTemp()
{

};
float ReadAccel()
{

};
float 

enum CANFrameAddress
{
    FL_CAN_FRAME_ADDRESS = 0x400,
    FR_CAN_FRAME_ADDRESS = 0x401,
    BL_CAN_FRAME_ADDRESS = 0x402,
    BR_CAN_FRAME_ADDRESS = 0x403
};