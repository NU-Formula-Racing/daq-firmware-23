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

void MotionBoard::setupMotionBoard() {
    //wire is needed for both
    Wire.begin();

    //IMU
    
    if(myIMU.begin() == false ) {
        Serial.print("IMU not detected at default I2C address. \n" );
    }
    if(! (myIMU.initialize(BASIC_SETTINGS)) ) {
        Serial.print("couldn't set up the basic settings for IMU \n" );
    } 
    
    //GPS
    if (myGNSS.begin() == false) //Connect to the u-blox module using Wire port
    {
        Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing. \n"));
        //while(1);
    }
    myGNSS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
    myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR
    
}

float* MotionBoard::ReadGPS()
{
    // Serial.print("lat:");
    long latitude = myGNSS.getLatitude() * 1e-07 ;
    // Serial.println(  myGNSS.getLatitude() * 1e-07 );
    // Serial.print("long: ");
    // Serial.print(myGNSS.getSIV());
    // Serial.println(  myGNSS.getLongitude()*1e-07);
    float lat = (float) myGNSS.getLatitude() *1e-07 ;
    float lon = (float) myGNSS.getLongitude()* 1e-07;
    //is this sus lol
    static float package[2] = {lat,lon};
    return package;
};
float* MotionBoard::ReadAccel()
{
    float x = (float) myIMU.readFloatAccelX();
    float y =  (float) myIMU.readFloatAccelY();
    float z = (float) myIMU.readFloatAccelZ();
    //     // Serial.print("accel x:");
    // Serial.println(x);
    // // Serial.print("accel_y:");
    // Serial.println(y);
    //    Serial.print("accel_z:");
    // Serial.println(z);
    static float package[3] = {x,y,z};
    return package;
};

float* MotionBoard::ReadGyro()
{
    float x = (float) myIMU.readFloatGyroX();
    float y =  (float) myIMU.readFloatGyroY();
    float z = (float) myIMU.readFloatGyroZ();
    static float package[3] = {x,y,z};
    return package;
};