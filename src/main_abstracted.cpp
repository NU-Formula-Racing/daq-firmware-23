// #include <Arduino.h>
// #include <SD.h>
// #include <SPI.h>
// //#include <esp32-hal-gpio.h>

// #include "RTClib.h"
// #include "virtualTimer.h"
// #include "can_interface.h"
// #include "virtualTimer.h"
// #include "..\lib\logger.h"

// RTC_PCF8523 rtc;

// // Initialize canbus based on teensy board
// #if defined(ARDUINO_TEENSY40) || defined(ARDUINO_TEENSY41)
// #include "teensy_can.h"
// // The bus number is a template argument for Teensy: TeensyCAN<bus_num>
// TeensyCAN<1> can_bus{};
// #endif

// #ifdef ARDUINO_ARCH_ESP32
// #include "esp_can.h"
// // The tx and rx pins are constructor arguments to ESPCan, which default to TX = 5, RX = 4; Currenlty tx=32, rx=27
// ESPCAN can_bus{GPIO_NUM_32, GPIO_NUM_27};
// #endif

// LoggerBoard logger;

// #if (logger.hp_lp == "LP")
// //
// //
// // START LP CAN
// //
// //

// //
// // LP CAN Addresses
// //

// const int kFL_CAN = 0x400;
// const int kFR_CAN = 0x401;
// const int kBL_CAN = 0x402;
// const int kBR_CAN = 0x403;
// const int kBrake_CAN = 0x410;
// const int kGPS_CAN = 0x430;
// const int kACCEL_CAN = 0x431;
// const int kGYRO_CAN = 0x432;

// //
// // LP CAN Messages
// //

// // Front left wheel speed and temp
// CANSignal<float, 0, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0), false> FL_wheel_speed_signal{};
// CANSignal<float, 16, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(-40), false> FL_brake_temp_signal{};
// CANSignal<float, 32, 16, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0.1), false> FL_suspension_position_signal{};
// CANRXMessage<2> rx_message_FLwheel{can_bus, kFL_CAN, FL_wheel_speed_signal, FL_brake_temp_signal};
// CANTXMessage<2> tx_message_FLwheel{can_bus, kFL_CAN, 4, 100, timer_group, FL_wheel_speed_signal, FL_brake_temp_signal};

// // Front right wheel speed and temp
// CANSignal<float, 0, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0), false> FR_wheel_speed_signal{};
// CANSignal<float, 16, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(-40), false> FR_brake_temp_signal{};
// CANSignal<float, 32, 16, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0.1), false> FR_suspension_position_signal{};
// CANRXMessage<3> rx_message_FRwheel{can_bus, kFR_CAN, FR_wheel_speed_signal, FR_brake_temp_signal, FR_suspension_position_signal};

// // Back left wheel speed and temp
// CANSignal<float, 0, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0), false> BL_wheel_speed_signal{};
// CANSignal<float, 16, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(-40), false> BL_brake_temp_signal{};
// CANSignal<float, 32, 16, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0.1), false> BL_suspension_position_signal{};
// CANRXMessage<3> rx_message_BLwheel{can_bus, kBL_CAN, BL_wheel_speed_signal, BL_brake_temp_signal, BL_suspension_position_signal};

// // Back right wheel speed and temp
// CANSignal<float, 0, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0), false> BR_wheel_speed_signal{};
// CANSignal<float, 16, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(-40), false> BR_brake_temp_signal{};
// CANSignal<float, 32, 16, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0.1), false> BR_suspension_position_signal{};
// CANRXMessage<3> rx_message_BRwheel{can_bus, kBR_CAN, BR_wheel_speed_signal, BR_brake_temp_signal, BR_suspension_position_signal};

// // Brake Pressure
// CANSignal<float, 0, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0.1), true> F_Brake_pressure{};
// CANSignal<float, 16, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0.1), true> R_Brake_pressure{};
// CANRXMessage<2> rx_message_brake{can_bus, kBrake_CAN, F_Brake_pressure, R_Brake_pressure};

// // // Acceleration
// CANSignal<float, 0, 16, CANTemplateConvertFloat(0.0005), CANTemplateConvertFloat(0), false> accel_x{};
// CANSignal<float, 16, 16, CANTemplateConvertFloat(0.0005), CANTemplateConvertFloat(0), false> accel_y{};
// CANSignal<float, 32, 16, CANTemplateConvertFloat(0.0005), CANTemplateConvertFloat(0), false> accel_z{};
// CANRXMessage<3> rx_message_accel{can_bus, kACCEL_CAN, accel_x, accel_y, accel_z};

// // Gyro
// CANSignal<float, 0, 16, CANTemplateConvertFloat(0.0005), CANTemplateConvertFloat(0), false> gyro_x{};
// CANSignal<float, 16, 16, CANTemplateConvertFloat(0.0005), CANTemplateConvertFloat(0), false> gyro_y{};
// CANSignal<float, 32, 16, CANTemplateConvertFloat(0.0005), CANTemplateConvertFloat(0), false> gyro_z{};
// CANRXMessage<3> rx_message_gyro{can_bus, kGYRO_CAN, gyro_x, gyro_y, gyro_z};

// // GPS
// CANSignal<float, 0, 16, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), true> lon_signal{};
// CANSignal<float, 32, 16, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), true> lat_signal{};
// CANRXMessage<2> rx_message_gps{can_bus, kGPS_CAN, lon_signal, lat_signal};

// // BMS Cell V Summary

// //
// //
// // END LP CAN
// //
// //
// #else
// //
// //
// // START HP CAN
// //
// //

// //
// // HP CAN Addresses
// //

// const int kTemp_CAN = 0x420;
// const int kThrottle_CAN = 0x300;

// //
// // HP CAN Messages
// //

// // Temp
// CANSignal<float, 0, 16, CANTemplateConvertFloat(0.01), CANTemplateConvertFloat(0), false> coolant_temp_signal{};
// CANSignal<float, 32, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(-40), false> ambient_temp_signal{};
// CANRXMessage<2> rx_message_temp{can_bus, kTemp_CAN, coolant_temp_signal, ambient_temp_signal};

// // BMS

// // Throttle
// CANSignal<uint8_t, 0, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), false> cur_throttle_signal{};
// CANRXMessage<1> rx_message_throttle{can_bus, kThrottle_CAN, cur_throttle_signal};

// //
// //
// // END HP CAN
// //
// //
// #endif

// void setup(){

// }

// void loop(){

// }
