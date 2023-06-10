#include <Arduino.h>
#include <SD.h>
#include <stephen.h>
#include <vector>

LoggerBoard::LoggerBoard(String fileName, String priority, std::map<std::string, float> sensor_10ms, std::map<std::string, float> sensor_100ms, std::map<std::string, float> sensor_1000ms)
{
  hp_lp = priority;
  dataString = "";
  fileName = "";

  if (hp_lp == "HP")
  {
    sensor_list = {"Motor Temp", "Coolant Temp", "Ambient Temp", "Remaining Battery", "Battery Temp", "Power Calc", "State of Charge"};
  }
  else if (hp_lp == "LP")
  {
    sensor_list = {"Timestamp", "Wheel_Speed", "Brake_Temp", "Accel_x", "Accel_y", "Accel_z", "Gyro_x", "Gyro_y", "Gyro_z", "Longitude", "Latitude"};
  }
  else
  {
    // ERROR
  }

  for (auto iter = sensor_list.begin(); iter != sensor_list.end(); ++iter)
  {
    dataString += *iter;
  }

  sensor_group_10ms = sensor_10ms;
  sensor_group_100ms = sensor_100ms;
  sensor_group_1000ms = sensor_1000ms;
};

void LoggerBoard::saveData()
{
  sensorData = SD.open(fileName, FILE_WRITE);
  if (sensorData)
  {
    // print line into csv
    sensorData.println(dataString.c_str());
    sensorData.close();
    Serial.println(dataString.c_str());
  }
  else
  {
    Serial.println("Error saving values to file !");
  }
};

DateTime LoggerBoard::init_RTC()
{
#ifndef ESP8266
  while (!Serial)
    ; // wait for serial port to connect. Needed for native USB
#endif

  Wire.begin(new_SDA, new_SCL);

  if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1)
      delay(10);
  }

  if (!rtc.initialized() || rtc.lostPower())
  {
    Serial.println("RTC is NOT initialized, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  rtc.start();
  DateTime now = rtc.now();

  float drift = 43;                                     // seconds plus or minus over oservation period - set to 0 to cancel previous calibration.
  float period_sec = (7 * 86400);                       // total obsevation period in seconds (86400 = seconds in 1 day:  7 days = (7 * 86400) seconds )
  float deviation_ppm = (drift / period_sec * 1000000); //  deviation in parts per million (μs)
  float drift_unit = 4.34;                              // use with offset mode PCF8523_TwoHours
  // float drift_unit = 4.069; //For corrections every min the drift_unit is 4.069 ppm (use with offset mode PCF8523_OneMinute)
  int offset = round(deviation_ppm / drift_unit);
  // rtc.calibrate(PCF8523_TwoHours, offset); // Un-comment to perform calibration once drift (seconds) and observation period (seconds) are correct
  // rtc.calibrate(PCF8523_TwoHours, 0); // Un-comment to cancel previous calibration

  Serial.print("Offset is ");
  Serial.println(offset); // Print to control offset

  return now;
};

void LoggerBoard::init_SD(DateTime now)
{
  SPIClass spi(VSPI);
  spi.begin(18, 19, 23, CSpin);
  spi.setDataMode(SPI_MODE0);
  pinMode(CSpin, OUTPUT);
  if (!SD.begin(CSpin, spi))
  {
    Serial.println("Card failed/not found.");
  }
  else
  {
    Serial.print("Card Initialized.");

    // New file name not working yet, does work with /test.txt
    fileName = "/test-" + String(now.month()) + "-" + String(now.day()) + "=" + String(now.hour()) + "-" + String(now.minute()) + ".csv";

    sensorData = SD.open(fileName, FILE_WRITE);

    if (sensorData)
    {
      dataString = "Timestamp, Ambient Temp, Motor Temp, Coolant Temp"; // Wheel_Speed, Brake_Temp, Accel_x, Accel_y, Accel_z, Gyro_x, Gyro_y, Gyro_z, Longitude, Latitude";
      saveData();
    }

    sensorData.close();
  }
};

void LoggerBoard::sensor10ms()
{
  DateTime now = rtc.now();

  // loop through list of 10ms_sensors, convert to C++
  for (auto const &iter : sensor_group_10ms)
  {
    dataString += "," + std::to_string(iter.second);
  }

  saveData();
};
