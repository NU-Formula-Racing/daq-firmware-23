#include <Arduino.h>
#include "../lib/CAN/include/virtualTimer.h"
#include "../lib/CAN/include/app_can.h"
#include "../../daqPinDefs.cpp"

#define SERIAL_DEBUG
#define ARDUINO_TEENSY40

#if defined(ARDUINO_TEENSY40) || defined(ARDUINO_TEENSY41)
#include "teensy_can.h"
// The bus number is a template argument for Teensy: TeensyCAN<bus_num>
TeensyCAN<1> can_bus{};
#endif

#ifdef ARDUINO_ARCH_ESP32
#include "esp_can.h"
// The tx and rx pins are constructor arguments to ESPCan, which default to TX = 5, RX = 4
ESPCan can_bus{};
#endif

// Initialize board
CANFrameAddress can_frame_address = FL_CAN_FRAME_ADDRESS;
WheelBoard wheel_board(1, 2, 3, can_frame_address);

//Structure for handling timers
virtualTimerGroup_S read_timer;

// TX CAN Message
CANSignal<uint16_t, 0, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0), true> wheel_speed_signal{}; 
CANSignal<uint16_t, 16, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(-40), true> brake_temp_signal{}; 
CANSignal<uint16_t, 32, 16, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), true> sus_pot_signal{}; 
CANTXMessage<3> tx_message{can_bus, can_frame_address, 4, std::chrono::milliseconds{100}, wheel_speed_signal, brake_temp_signal, sus_pot_signal};

// Wheel speed sensor
unsigned long current_pulse_time = 0;
unsigned long previous_pulse_time = 0;
unsigned long pulse_duration = 0;

/**
 * @brief Runs on interrupt to record pulse duration
 * 
 */
void ReadWheelSpeedSensorDuration()
{
    current_pulse_time = micros();
    pulse_duration = current_pulse_time - previous_pulse_time;
    previous_pulse_time = current_pulse_time;
}

/**
 * @brief Uses the pulse duration to calculate the wheel speed in MPH
 * 
 */
void ReadWheelSpeedSensor()
{
    // Wheel constants
    const float kWheelCircumference = 1.26; //wheel circumference in m
    const float kMPStoMPH = 2.24; // m/s to MPH constant 
    const int kMagnetCount = 20;
    //We multiply by 1M because our pulseDuration is measured in uS
    float pulse_frequency = 1000000.0 / (float)pulse_duration;
    float wheel_frequency = pulse_frequency / (float)kMagnetCount; 
    wheel_speed_signal = (uint16_t)(wheel_frequency * kWheelCircumference * kMPStoMPH);
}

// Brake temperature
/**
 * @brief Reads the brake temperature sensor ADC value and converts it to temperature units (degrees C)
 * 
 */
void ReadBrakeTempSensor()
{
  uint16_t raw_ADC_value = 0;
  raw_ADC_value = analogRead(wheel_board.brake_temp_sensor_pin);
  brake_temp_signal = raw_ADC_value * wheel_board.brake_temp_scalar + wheel_board.brake_temp_offset;
}

// Suspension position sensor
/**
 * @brief Reads the suspension position sensor ADC value and converts it to length (mm)
 * 
 */
void ReadSuspensionPositionSensor()
{
    uint16_t raw_ADC_value = 0;
    raw_ADC_value = analogRead(SUSPENSION_POSITION_SENSOR_PIN);
    sus_pot_signal = raw_ADC_value * SUSPENSION_POSITION_SCALAR + SUSPENSION_POSITION_OFFSET;
}

/**
 * @brief Interrupt function for handling wheel speed
 * 
 */
void IRAM_ATTR WheelSpeedISR() {
	ReadWheelSpeedSensorDuration();
}

void setup() {

  #ifdef SERIAL_DEBUG
  //Initialize serial output 
  Serial.begin(115200);
  #endif

  //This only works on ESP32, will crash on compile for Teensy
  //This makes us trigger reading wheel speed in an interrupt 
  attachInterrupt(WHEEL_SPEED_SENSOR_PIN, WheelSpeedISR, RISING); 

  //Initialize our timer(s)
  read_timer.addTimer(100, ReadWheelSpeedSensor);
  read_timer.addTimer(100, ReadBrakeTempSensor);
  read_timer.addTimer(1000, ReadSuspensionPositionSensor);

  can_bus.Initialize(ICAN::BaudRate::kBaud1M);
}

void loop() {
  read_timer.tick(millis());
  tx_message.Tick(millis());
}

