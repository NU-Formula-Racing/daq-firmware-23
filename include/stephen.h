#include "RTClib.h"
#include <map>
#include <list>
#include <Arduino.h>
#include <SD.h>
#include <SPI.h>

class LoggerBoard
{
public:
    LoggerBoard(String fileName, String priority, std::map<std::string, float> sensor_10ms, std::map<std::string, float> sensor_100ms, std::map<std::string, float> sensor_1000ms);

    void getPriority();

    void saveData();
    DateTime init_RTC();
    void init_SD(DateTime now);

    void sensor10ms();

private:
    String hp_lp;
    String fileName;
    std::string dataString;
    File sensorData;
    RTC_PCF8523 rtc;
    const int CSpin = 5;
    const int new_SDA = 21;
    const int new_SCL = 22;
    std::list<std::string> sensor_list;
    std::map<std::string, float> sensor_group_10ms;
    std::map<std::string, float> sensor_group_100ms;
    std::map<std::string, float> sensor_group_1000ms;
};