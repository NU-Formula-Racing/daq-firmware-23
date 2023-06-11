#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <esp32-hal-gpio.h>

#include "RTClib.h"
#include "virtualTimer.h"
#include "can_interface.h"
#include "virtualTimer.h"
#include "inverter_driver.h"
#include <iostream>
#include <string>

RTC_PCF8523 rtc;
VirtualTimerGroup timer_group;
enum SDState
{
  OPENED = 0,
  CLOSED = 1
};
SDState current_state = SDState::CLOSED;
SDState previous_state = SDState::CLOSED;

// Initialize canbus based on teensy board
#if defined(ARDUINO_TEENSY40) || defined(ARDUINO_TEENSY41)
#include "teensy_can.h"
// The bus number is a template argument for Teensy: TeensyCAN<bus_num>
TeensyCAN<1>
    can_bus{};
#endif

#ifdef ARDUINO_ARCH_ESP32
#include "esp_can.h"
// The tx and rx pins are constructor arguments to ESPCan, which default to TX = 5, RX = 4; Currenlty tx=32, rx=27
ESPCAN can_bus{10, GPIO_NUM_32, GPIO_NUM_27};
#endif

#define isLP (false)

#if isLP

//
// LP CAN Addresses
//

const int kFL_Wheel = 0x400;
const int kFR_Wheel = 0x401;
const int kBL_Wheel = 0x402;
const int kBR_Wheel = 0x403;
const int kBrake_Pressure = 0x410;
const int kPtrain_Temps = 0x420;
const int kGPS = 0x430;
const int kACCEL = 0x431;
const int kGYRO = 0x432;
const int kPDM_Rails = 0x500;
const int kPDM_Devies = 0x501;

//
// LP CAN Messages
//

////////////////////////////////////////////////////////////////
// 10 ms

// Front Left Wheel
CANSignal<float, 0, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0), false> fl_wheel_speed_signal{};
CANSignal<float, 16, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(-40), false> fl_brake_temp_signal{};
CANRXMessage<2> FL_Wheel_RX_Message{can_bus, kFL_Wheel, fl_wheel_speed_signal, fl_brake_temp_signal};

// Front Right Wheel
CANSignal<float, 0, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0), false> fr_wheel_speed_signal{};
CANSignal<float, 16, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(-40), false> fr_brake_temp_signal{};
CANRXMessage<2> FR_Wheel_RX_Message{can_bus, kFR_Wheel, fr_wheel_speed_signal, fr_brake_temp_signal};

// Back Left Wheel
CANSignal<float, 0, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0), false> bl_wheel_speed_signal{};
CANSignal<float, 16, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(-40), false> bl_brake_temp_signal{};
CANRXMessage<2> BL_Wheel_RX_Message{can_bus, kBL_Wheel, bl_wheel_speed_signal, bl_brake_temp_signal};

// Back Right Wheel
CANSignal<float, 0, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0), false> br_wheel_speed_signal{};
CANSignal<float, 16, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(-40), false> br_brake_temp_signal{};
CANRXMessage<2> BR_Wheel_RX_Message{can_bus, kBR_Wheel, br_wheel_speed_signal, br_brake_temp_signal};

// Brake Pressure
CANSignal<float, 0, 16, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), false> f_brake_pressure_signal{};
CANSignal<float, 16, 16, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), false> b_brake_pressure_signal{};
CANRXMessage<2> Brake_Pressure_RX_Message{can_bus, kBrake_Pressure, f_brake_pressure_signal, b_brake_pressure_signal};

// Accelerometer
CANSignal<float, 0, 16, CANTemplateConvertFloat(0.0005), CANTemplateConvertFloat(0), true> accel_x_signal{};
CANSignal<float, 16, 16, CANTemplateConvertFloat(0.0005), CANTemplateConvertFloat(0), true> accel_y_signal{};
CANSignal<float, 32, 16, CANTemplateConvertFloat(0.0005), CANTemplateConvertFloat(0), true> accel_z_signal{};
CANRXMessage<3> Accelerometer_RX_Message{can_bus, kACCEL, accel_x_signal, accel_y_signal, accel_z_signal};

// Gyro
CANSignal<float, 0, 16, CANTemplateConvertFloat(0.0005), CANTemplateConvertFloat(0), true> gyro_x_signal{};
CANSignal<float, 16, 16, CANTemplateConvertFloat(0.0005), CANTemplateConvertFloat(0), true> gyro_y_signal{};
CANSignal<float, 32, 16, CANTemplateConvertFloat(0.0005), CANTemplateConvertFloat(0), true> gyro_z_signal{};
CANRXMessage<3> Gyroscope_RX_Message{can_bus, kGYRO, gyro_x_signal, gyro_y_signal, gyro_z_signal};

////////////////////////////////////////////////////////////////
// 100 ms

// Power Train Temperatures
CANSignal<float, 0, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(-40), false> motor_temp_signal{};
CANSignal<float, 16, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(-40), false> coolant_temp_signal{};
CANSignal<float, 32, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(-40), false> ambient_temp_signal{};
CANRXMessage<3> Ptrain_Temps_RX_Message{can_bus, kPtrain_Temps, motor_temp_signal, coolant_temp_signal, ambient_temp_signal};

// GPS
CANSignal<float, 0, 32, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), true> lat_signal{};
CANSignal<float, 32, 32, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), true> lon_signal{};
CANRXMessage<2> GPS_RX_Message{can_bus, kGPS, lat_signal, lon_signal};

// PDM Rails
CANSignal<float, 0, 8, CANTemplateConvertFloat(0.02), CANTemplateConvertFloat(0), false> v5_rail_signal{};
CANSignal<float, 8, 8, CANTemplateConvertFloat(0.02), CANTemplateConvertFloat(0), false> v12_rail_signal{};
CANSignal<float, 16, 8, CANTemplateConvertFloat(0.02), CANTemplateConvertFloat(0), false> vbat_rail_signal{};
CANSignal<float, 24, 16, CANTemplateConvertFloat(0.002), CANTemplateConvertFloat(0), true> vbat_input_current_signal{};
CANSignal<float, 40, 8, CANTemplateConvertFloat(0.05), CANTemplateConvertFloat(10), false> vbat_input_voltage_signal{};
CANRXMessage<5> PDM_Rails_RX_Message{can_bus, kPDM_Rails, v5_rail_signal, v12_rail_signal, vbat_rail_signal, vbat_input_current_signal, vbat_input_voltage_signal};

// PDM Devices
CANSignal<float, 0, 8, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0), false> air_fan_signal{};
CANSignal<float, 8, 8, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0), false> liquid_fan_signal{};
CANSignal<float, 16, 8, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0), false> liquid_pump_signal{};
CANSignal<float, 24, 8, CANTemplateConvertFloat(0.02), CANTemplateConvertFloat(0), false> hsd_1_signal{};
CANSignal<float, 32, 8, CANTemplateConvertFloat(0.02), CANTemplateConvertFloat(0), false> hsd_2_signal{};
CANRXMessage<5> PDM_Devices_RX_Message{can_bus, kPDM_Devies, air_fan_signal, liquid_fan_signal, liquid_pump_signal, hsd_1_signal, hsd_2_signal};

#else

//
// HP CAN Addresses
//

const int kThrottle_Values = 0x300;
const int kBMS_SOE = 0x240;
const int kBMS_Faults = 0x250;
const int kBMS_Status = 0x241;

//
// HP CAN Messages
//

////////////////////////////////////////////////////////////////
// 10 ms

// Throttle Values
CANSignal<int8_t, 0, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), true> accel_percentage_signal{};
CANSignal<uint8_t, 8, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), false> brake_percentage_signal{};
CANSignal<uint8_t, 16, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), false> torque_limit_percentage_signal{};
CANRXMessage<3> Throttle_Values_RX_Message{can_bus, kThrottle_Values, accel_percentage_signal, brake_percentage_signal, torque_limit_percentage_signal};

// BMS SOE
CANSignal<float, 0, 12, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0), false> max_discharge_current_signal{};
CANSignal<float, 12, 12, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0), false> max_regen_current_signal{};
CANSignal<float, 24, 16, CANTemplateConvertFloat(0.01), CANTemplateConvertFloat(0), false> battery_voltage_signal{};
CANSignal<float, 40, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(-40), false> battery_temperature_signal{};
CANSignal<float, 48, 16, CANTemplateConvertFloat(0.01), CANTemplateConvertFloat(0), true> battery_current_signal{};
CANRXMessage<5> BMS_SOE_RX_Message{can_bus, kBMS_SOE, max_discharge_current_signal, max_regen_current_signal, battery_voltage_signal, battery_temperature_signal, battery_current_signal};

////////////////////////////////////////////////////////////////
// 100 ms

// BMS Faults
CANSignal<bool, 0, 1, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), false> fault_summary_signal{};
CANSignal<bool, 1, 1, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), false> undervoltage_fault_signal{};
CANSignal<bool, 2, 1, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), false> overvoltage_fault_signal{};
CANSignal<bool, 3, 1, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), false> undertemperature_fault_signal{};
CANSignal<bool, 4, 1, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), false> overtemperature_fault_signal{};
CANSignal<bool, 5, 1, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), false> overcurrent_fault_signal{};
CANSignal<bool, 6, 1, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), false> external_kill_fault_signal{};
CANSignal<bool, 7, 1, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), false> open_wire_fault_signal{};
CANRXMessage<8> BMS_Faults_RX_Message{can_bus, kBMS_Faults, fault_summary_signal, undervoltage_fault_signal, overvoltage_fault_signal, undertemperature_fault_signal, overtemperature_fault_signal, overcurrent_fault_signal, external_kill_fault_signal, open_wire_fault_signal};

// BMS Status
CANSignal<uint8_t, 0, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), false> bms_state_signal{};
CANSignal<float, 8, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(-40), false> max_cell_temp_signal{};
CANSignal<float, 16, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(-40), false> min_cell_temp_signal{};
CANSignal<float, 24, 8, CANTemplateConvertFloat(0.012), CANTemplateConvertFloat(2), false> max_cell_voltage_signal{};
CANSignal<float, 32, 8, CANTemplateConvertFloat(0.012), CANTemplateConvertFloat(2), false> min_cell_voltage_signal{};
CANSignal<uint8_t, 40, 8, CANTemplateConvertFloat(0.5), CANTemplateConvertFloat(0), false> bms_soc_signal{};
CANRXMessage<6> BMS_Status_RX_Message{can_bus, kBMS_Status, bms_state_signal, max_cell_temp_signal, min_cell_temp_signal, max_cell_voltage_signal, min_cell_voltage_signal, bms_soc_signal};

#endif

const int kRTC = 0x440;

// Send RTC Data
CANSignal<uint32_t, 0, 32, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), false> rtc_signal{};
// Transmit RTC Data.
CANTXMessage<1> tx_message{can_bus, kRTC, 4, 10, timer_group, rtc_signal};

const int CSpin = 5;
const int new_SDA = 21;
const int new_SCL = 22;

File sensorData;
char fileName[] = "/date=00-00-00--time=00-00-00.csv";
String dataString = "";
int log_count = 1;

Inverter inverter{can_bus};

/**
 * @brief Save current dataString to file.
 *
 */
void saveData()
{
  // Reopen file.
  sensorData = SD.open(fileName, FILE_APPEND);
  if (sensorData)
  {
    // print line into csv
    sensorData.println(dataString);
    // Serial.println(dataString);
    sensorData.close();
  }
  else
  {
    Serial.print("Error saving values to file ! \n");
  }
}

/**
 * @brief Save sensor information if the SD card is in an open state.
 * The saved string depends on the frequency (10 hz, 100hz, and 1000hz)
 * of each sensor, and whether it is on LP CAN and HP CAN.
 *
 */
void sensorLog()
{
  if (current_state == SDState::OPENED)
  {

    DateTime now = rtc.now();
    String placeholder = "";
    // Send RTC signal through CAN.
    rtc_signal = now.unixtime();

#if isLP

    // 100 ms
    if (log_count % 10 == 0)
    {
      dataString = placeholder + now.timestamp() + ", " + float(fl_wheel_speed_signal) + ", " + float(fl_brake_temp_signal) + ","
      + float(fr_wheel_speed_signal) + ", " + float(fr_brake_temp_signal) + ","
      + float(bl_wheel_speed_signal) + ", " + float(bl_brake_temp_signal) + ","
      + float(br_wheel_speed_signal) + ", " + float(br_brake_temp_signal) + ","
      + float(f_brake_pressure_signal) + ", " + float(b_brake_pressure_signal) + ","
      + float(accel_x_signal) + ", " + float(accel_y_signal) + "," + float(accel_z_signal) + ","
      + float(gyro_x_signal) + ", " + float(gyro_y_signal) + "," + float(gyro_z_signal) + ","
      + float(motor_temp_signal) + ", " + float(coolant_temp_signal) + "," + float(ambient_temp_signal) + "," 
      + float(lat_signal) + ", " + float(lon_signal) + ","
      + float(v5_rail_signal) + ", " + float(v12_rail_signal) + "," + float(vbat_rail_signal) + "," + float(vbat_input_current_signal) + "," + float(vbat_input_voltage_signal) + "," 
      + float(air_fan_signal) + ", " + float(liquid_fan_signal) + "," + float(liquid_pump_signal) + "," + float(hsd_1_signal) + "," + float(hsd_2_signal) + "," 
      + "100ms"; 
    }
    // 10 ms
    else
    {
      dataString = placeholder + now.timestamp() + ", " + float(fl_wheel_speed_signal) + ", " + float(fl_brake_temp_signal) + ","
      + float(fr_wheel_speed_signal) + ", " + float(fr_brake_temp_signal) + ","
      + float(bl_wheel_speed_signal) + ", " + float(bl_brake_temp_signal) + ","
      + float(br_wheel_speed_signal) + ", " + float(br_brake_temp_signal) + ","
      + float(f_brake_pressure_signal) + ", " + float(b_brake_pressure_signal) + ","
      + float(accel_x_signal) + ", " + float(accel_y_signal) + "," + float(accel_z_signal) + ","
      + float(gyro_x_signal) + ", " + float(gyro_y_signal) + "," + float(gyro_z_signal) + ","
      + " , , ," 
      + " , ," 
      + " , , , , ," 
      + " , , , , ,"  
      + "10ms"; 
    }

#else
    // 100 ms
    if (log_count % 100 == 0)
    {
      // dataString = placeholder + now.timestamp() + ", " + inverter.GetRPM() + ", " + inverter.GetInverterTemperature() + ", " + inverter.GetMotorTemperature() + "," + float(coolant_temp_signal) + "," + float(ambient_temp_signal) + ", " + float(cur_throttle_signal);
      dataString = placeholder + now.timestamp() + ", " + + float(accel_percentage_signal) + ", " + float(brake_percentage_signal) + "," + float(torque_limit_percentage_signal) + ","
      + float(max_discharge_current_signal) + "," + float(max_regen_current_signal) + "," + float(battery_voltage_signal) + "," + float(battery_temperature_signal) + "," + float(battery_current_signal) + "," 
      + float(fault_summary_signal) + "," + float(undervoltage_fault_signal) + "," + float(overvoltage_fault_signal) + "," + float(undertemperature_fault_signal) + "," + float(overtemperature_fault_signal) + ","  + float(overcurrent_fault_signal) + "," + float(external_kill_fault_signal) + "," + float(open_wire_fault_signal) + "," 
      + float(bms_state_signal) + "," + float(max_cell_temp_signal) + "," + float(min_cell_temp_signal) + "," + float(max_cell_voltage_signal) + "," + float(min_cell_voltage_signal) + ","  + float(bms_soc_signal) + "," 
      + "100ms"; 
    }
    // 10 ms 
    else
    {
      // dataString = placeholder + now.timestamp() + ", " + inverter.GetRPM() + ", " + inverter.GetInverterTemperature() + ", " + inverter.GetMotorTemperature() + ", , , " + float(cur_throttle_signal);
      dataString = placeholder + now.timestamp() + ", " + + float(accel_percentage_signal) + ", " + float(brake_percentage_signal) + "," + float(torque_limit_percentage_signal) + ","
      + float(max_discharge_current_signal) + "," + float(max_regen_current_signal) + "," + float(battery_voltage_signal) + "," + float(battery_temperature_signal) + "," + float(battery_current_signal) + "," 
      +  " , , , , , , , , " 
      +  " , , , , , , " 
      + "100ms"; 
    }
#endif

    saveData();
    log_count += 1;
  }
}

//
// Start the Setup
//

DateTime init_RTC()
{
#ifndef ESP8266
  while (!Serial)
    ; // wait for serial port to connect. Needed for native USB
#endif

  Wire.begin(new_SDA, new_SCL);

  if (!rtc.begin())
  {
    Serial.print("Couldn't find RTC \n");
    Serial.flush();
    while (1)
      delay(10);
  }

  if (!rtc.initialized() || rtc.lostPower())
  {
    Serial.print("RTC is NOT initialized, let's set the time! \n");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  rtc.start();
  // Turn on if RTC drifts too far
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  DateTime now = rtc.now();

  float drift = 10;                                     // seconds plus or minus over observation period - set to 0 to cancel previous calibration.
  float period_sec = (7 * 86400);                       // total observation period in seconds (86400 = seconds in 1 day:  7 days = (7 * 86400) seconds )
  float deviation_ppm = (drift / period_sec * 1000000); //  deviation in parts per million (μs)
  float drift_unit = 4.34;                              // use with offset mode PCF8523_TwoHours
  // float drift_unit = 4.069; //For corrections every min the drift_unit is 4.069 ppm (use with offset mode PCF8523_OneMinute)
  int offset = round(deviation_ppm / drift_unit);
  rtc.calibrate(PCF8523_TwoHours, offset); // Un-comment to perform calibration once drift (seconds) and observation period (seconds) are correct
  // rtc.calibrate(PCF8523_TwoHours, 0); // Un-comment to cancel previous calibration

  // Print control offset.
  Serial.print("Offset is " + String(offset) + "\n");

  return now;
}

/**
 * @brief Create new file with adequate name and write header to file.
 *
 * @param now: current time from RTC
 */
void init_SD(DateTime now)
{
  SPI.begin(18, 19, 23, CSpin);
  SPI.setDataMode(SPI_MODE0);
  pinMode(CSpin, OUTPUT);
  if (!SD.begin(CSpin, SPI))
  {
    Serial.print("Card failed/not found. \n");
  }
  else
  {
    Serial.print("init_SD Initialization Starting... \n");
    String priority;
#if isLP
    priority = "LP";
#else
    priority = "HP";
#endif
    sprintf(fileName, "/%s-date=%hhu-%hhu-%hu-time=%hhu-%hhu-%hhu.csv", priority, now.month(), now.day(), now.year(), now.hour(), now.minute(), now.second());
    // Create file and close it.
    Serial.println(fileName);
    sensorData = SD.open(fileName, FILE_WRITE);
    sensorData.close();
#if isLP
    dataString = "Timestamp, FL Wheel Speed, FL Brake Temp, FR Wheel Speed, FR Brake Temp, BL Wheel Speed, BL Brake Temp, BR Wheel Speed, BR Brake Temp, Motor Temp, Coolant Temp, Ambient Temp, Latitude, Longitude, Accel X, Accel Y, Accel Z, Gyro X, Gyro Y, Gyro Z, F Brake Pressure, B Brake Pressure, V5 Rail, V12 Rail, VBat Rail, VBat Input Current, VBat Input Voltage, Air Cooling Fan, Liquid Cooling Fan, Liquid Cooling Pump, High Side Driver 1, High Side Driver 2, Logging Timeframe";
#else
    dataString = "Timestamp, Accel Percentage, Brake Percentage, Torque Limit Percentage, Max Discharge Current, Max Regen Current, Battery Voltage, Battery Temperature, Battery Current, Fault Summary, Undervoltage Fault, Overvoltage Fault, Undertemperature Fault, Overtemperature Fault, Overcurrent Fault, External Kill Fault, Open Wire Fault, BMS State, Max Cell Temp, Min Cell Temp, Max Cell Voltage, Min Cell Voltage, BMS SOC, Logging Timeframe";
#endif
    saveData();
    Serial.print("init_SD Initialization Finished \n");
  }
}

void eject_SD()
{
  Serial.print("Ejecting SD... \n");
  SD.end();
  Serial.print("SD Ejected \n");
}

/**
 * @brief Change the state of the SD when
 * the eject button is pressed on the ESP32.
 *
 */
void changeSDState()
{
  if (current_state == SDState::OPENED)
  {
    current_state = SDState::CLOSED;
    previous_state = SDState::OPENED;
  }
  else
  {
    current_state = SDState::OPENED;
    previous_state = SDState::CLOSED;
  }
}

/**
 * @brief Initialize SD card or eject the SD card based
 * on the previous state and current state of the SD card.
 *
 */
void detectSDState()
{
  if (previous_state == SDState::CLOSED && current_state == SDState::OPENED)
  {
    init_SD(rtc.now());
    previous_state = current_state;
  }
  else if (previous_state == SDState::OPENED && current_state == SDState::CLOSED)
  {
    eject_SD();
    previous_state = current_state;
  }
}

void setup(void)
{
  Serial.begin(9600);

  pinMode(GPIO_NUM_26, OUTPUT);
  digitalWrite(GPIO_NUM_26, LOW);
  pinMode(GPIO_NUM_33, OUTPUT);
  digitalWrite(GPIO_NUM_33, LOW);

  Serial.print("Initializing CAN... \n");
  can_bus.Initialize(ICAN::BaudRate::kBaud1M);

#if isLP
  //
  // LP CAN Messages
  //
  can_bus.RegisterRXMessage(FL_Wheel_RX_Message);
  can_bus.RegisterRXMessage(FR_Wheel_RX_Message);
  can_bus.RegisterRXMessage(BL_Wheel_RX_Message);
  can_bus.RegisterRXMessage(BR_Wheel_RX_Message);
  can_bus.RegisterRXMessage(Brake_Pressure_RX_Message);
  can_bus.RegisterRXMessage(Accelerometer_RX_Message);
  can_bus.RegisterRXMessage(Gyroscope_RX_Message);
  can_bus.RegisterRXMessage(Ptrain_Temps_RX_Message);
  can_bus.RegisterRXMessage(GPS_RX_Message);
  can_bus.RegisterRXMessage(PDM_Rails_RX_Message);
  can_bus.RegisterRXMessage(PDM_Devices_RX_Message);

#else
  //
  // HP CAN Messages
  //
  can_bus.RegisterRXMessage(Throttle_Values_RX_Message);
  can_bus.RegisterRXMessage(BMS_SOE_RX_Message);
  can_bus.RegisterRXMessage(BMS_Faults_RX_Message);
  can_bus.RegisterRXMessage(BMS_Status_RX_Message);

#endif

  Serial.print("CAN Initialized \n");

#if isLP
  Serial.print("No inverter for LP \n");

#else
  Serial.print("Initializing Inverter... \n");
  inverter.RequestMotorTemperature(100);
  inverter.RequestPowerStageTemp(100);
  inverter.RequestRPM(100);
  Serial.print("Inverter Initialized \n");

#endif

  Serial.print("Initializing RTC... \n");
  DateTime now = init_RTC();
  Serial.print("RTC Initialized \n");

  attachInterrupt(0, changeSDState, FALLING);
  SD.end();
  timer_group.AddTimer(1000, []()
                       { Serial.printf("current state: %d \n", current_state); });
  timer_group.AddTimer(100, detectSDState);

  Serial.print("Initializing Timers... \n");
  timer_group.AddTimer(10, sensorLog);
  Serial.print("Timers Initialized \n");
}

void loop()
{
  can_bus.Tick();
  timer_group.Tick(millis());
}
