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
//lattitude 
float MotionBoard::ReadLat() {
    float lat = (float) myGNSS.getLatitude();
    return lat;
}
//longitude
float MotionBoard::ReadLon() {
    float lon = (float) myGNSS.getLongitude();
    return lon;
}

//units in g
float MotionBoard::ReadXAccel() {
    return myIMU.readFloatAccelX();
};
float MotionBoard::ReadYAccel() {
    return myIMU.readFloatAccelY();
};
float MotionBoard::ReadZAccel() {
    return myIMU.readFloatAccelZ();
};

//Gyro units in degrees per second (dps)
float MotionBoard::ReadXGyro() {
    return myIMU.readFloatGyroX();
};

float MotionBoard::ReadYGyro() {
    return myIMU.readFloatGyroY();
}
float MotionBoard::ReadZGyro() {
    return myIMU.readFloatGyroZ();
}
