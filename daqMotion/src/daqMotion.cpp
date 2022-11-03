#include <Arduino.h>
#include <daqMotion.h>
//GPS includes 
#include <Wire.h> //Needed for I2C to GNSS
#include "SparkFunLSM6DSO.h"
#include <SparkFun_u-blox_GNSS_Arduino_Library.h> //http://librarymanager/All#SparkFun_u-blox_GNSS
SFE_UBLOX_GNSS myGNSS;

long lastTime = 0; //Simple local timer. Limits amount if I2C traffic to u-blox module.
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
float MotionBoard::ReadAccel()
{
    float hello = 10; 
    return hello;
};