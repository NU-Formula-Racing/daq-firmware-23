#include <Arduino.h>
#include "daqWheel.h"

#define SERIAL_DEBUG

#if defined(ARDUINO_TEENSY40) || defined(ARDUINO_TEENSY41)
#include "teensy_can.h"
// The bus number is a template argument for Teensy: TeensyCAN<bus_num>
TeensyCAN<1> can_bus{};
#endif

#ifdef ARDUINO_ARCH_ESP32
#include "esp_can.h"
// The tx and rx pins are constructor arguments to ESPCan, which default to TX = 5, RX = 4
ESPCAN can_bus{};
#endif

// Choose values based on the sensors' location on the vehicle. 
// Choose the front or back wheel speed and brake temperature sensors. 
// true: front 
// false: back 
#define isFront (true)
// Choose the right or left wheel speed and brake temperature sensors for
// the selected location (front or back).
// true: right 
// false: left
#define isRight (true)

// Initialize board
WheelSpeedBoard wheel_speed_board;

// Structure for handling timers
VirtualTimerGroup read_timer;

// CAN frame address for each wheel. 
enum CANFrameAddress
{
    FRONT_LEFT = 0x400,
    FRONT_RIGHT = 0x401,
    BACK_LEFT = 0x402,
    BACK_RIGHT = 0x403
};

// Front of vehicle.
#if isFront 

    // Bool passed to ReadWheelSpeedSensor().
    bool frontLocation = true;

    // Front right of vehicle.
    #if isRight
    auto SelectedCANAddress = CANFrameAddress::FRONT_RIGHT;
    // Front left of vehicle.  
    #else 
    auto SelectedCANAddress = CANFrameAddress::FRONT_LEFT;
    #endif

// Back of vehicle.
# else

    // Bool passed to ReadWheelSpeedSensor().
    bool frontLocation = false;

    // Back right of vehicle.
    #if isRight
    auto SelectedCANAddress = CANFrameAddress::BACK_RIGHT;
    // Back left of vehicle.  
    #else 
    auto SelectedCANAddress = CANFrameAddress::BACK_LEFT;
    #endif

#endif

// TX CAN Message
CANSignal<float, 0, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0), false> wheel_speed_signal{};
CANSignal<float, 16, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(-40), false> brake_temp_signal{};
CANTXMessage<2> WHEEL_TX_MESSAGE{can_bus, SelectedCANAddress, 4, 10, read_timer, wheel_speed_signal, brake_temp_signal};


/**
 * @brief Read and record wheel speed CAN value depending on location.
 * 
 */
void ReadWheelSpeedSensor()
{
    wheel_speed_signal = wheel_speed_board.ReadWheelSpeedSensor(frontLocation);
    Serial.printf("Wheel Speed = %0.2f MPH\n", wheel_speed_signal);
}

/**
 * @brief Read and record brake temperature CAN value depending on location.
 * 
 */
void ReadBrakeTempSensor()
{
    brake_temp_signal = wheel_speed_board.ReadBrakeTempSensor();
    Serial.printf("Brake Temperature = %0.2f Celsius\n", brake_temp_signal);
}

void IRAM_ATTR WheelSpeedISR() { wheel_speed_board.ReadWheelSpeedSensorDuration(); }

void setup()
{
#ifdef SERIAL_DEBUG
    // Initialize serial output
    Serial.begin(9600);
#endif

    // This only works on ESP32, will crash on compile for Teensy
    // This makes us trigger reading wheel speed in an interrupt
    // Magnets are flipped so we both a rising and falling edge have
    // to be detected.
    attachInterrupt(wheel_speed_board.wheelSpeedSensorPin, WheelSpeedISR, RISING);
    attachInterrupt(wheel_speed_board.wheelSpeedSensorPin, WheelSpeedISR, FALLING);

    // Initialize can bus
    can_bus.Initialize(ICAN::BaudRate::kBaud1M);

    // Initialize our timer(s).
    read_timer.AddTimer(10, ReadWheelSpeedSensor);
    read_timer.AddTimer(10, ReadBrakeTempSensor);
}

void loop() 
{
    read_timer.Tick(millis()); 
}
