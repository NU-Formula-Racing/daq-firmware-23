#include <Arduino.h>
#include <daqMotion.h>
//GPS includes 
#include <Wire.h> //Needed for I2C to GNSS
#include "SparkFunLSM6DSO.h"
#include <SparkFun_u-blox_GNSS_Arduino_Library.h> //http://librarymanager/All#SparkFun_u-blox_GNSS


#include "Wire.h"

long lastTime = 0; //Simple local timer. Limits amount if I2C traffic to u-blox module.
/**
 * @brief Construct a new Wheel Board:: Wheel Board object
 * 
 * @param can_frame 
 */

SFE_UBLOX_GNSS myGNSS;
LSM6DSO myIMU;

MotionBoard::MotionBoard()
{
    current_pulse_time = 0;
    previous_pulse_time = 0;
    pulse_duration = 0;
};

bool MotionBoard::setupMotionBoard() {
    Wire.begin();
    //IMU
    if( !(myIMU.begin()) ) {
        //Serial.print if you can
         return false;
    }
    if(! (myIMU.initialize(BASIC_SETTINGS)) ) {
        //Serial.print if you can
        return false;
    }
    if (myGNSS.begin() == false) //Connect to the u-blox module using Wire port
    {
        //Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
        return false;
        //while (1);
    }

    myGNSS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
    myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR


    return true; 
}
/**
 */
float MotionBoard::ReadGPS()
{
    long latitude = myGNSS.getLatitude();
    return latitude;
};
float MotionBoard::ReadAccel()
{
    return myIMU.readFloatAccelY();
};