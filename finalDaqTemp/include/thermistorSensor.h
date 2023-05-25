/* thermistorSensor.h*/

#include "ISensor.h"
// * I realize that this abstraction is very overkill, but this is more for the sake that we are working as a team
// * This, in some circles, is considered best practice; it is best to ensure that our code follows best practices for the sake of following years

//
// The ThermistorSensor object is an abstraction of a hardware implementation where a thermistor acts as R1 in a voltage divider
// To effectively use this abstraction, the user must knoww 4 things about the hardware:
//  - The Steinhart-hart Coefficents for the given thermistor
//      - Review: https://en.wikipedia.org/wiki/Steinhart%E2%80%93Hart_equation for information
//      - The default values are set to 2023's ambient temperature sensor values
//      - If the user wishes to deviate from these values, please reference the SetSteinhartHartCoefficents() method
//  - The input voltage inputted in the voltage divider - the base implementation sets 3V3 has the default
//  - The resolution of the mircocontroller in regards to the analaogRead() method (what is the max value?) - 4065 is set to the default as it is the ESP32's resolution
//  - The pin that the connector is set on - this is passed through the resistor, along with the value of the second resistor
// If the user wishes to deviate from the standard hardware layout in terms of value, reference the SetHardwareConfig() method
//
class ThermistorSensor : public ISensor
{
    private:
        // Steinhart-hart Coefficients - Default is set to 2023's ambient sensor 
        float _a = 0.0008646457;
        float _b = 0.0002546929;
        float _c = 0.0000001693933;

        // Electrical Configuration
        float _r2;
        int _sensorPin;
        float _vcc = 3.3;
        float _vccRes = 4095;

    public:
        //
        // Create a ThermistorSensor object who's input is read at "sensorPin", and uses a resistor of "r2" ohms as the known resistor value
        //
        ThermistorSensor(int sensorPin, float r2):
        _sensorPin(sensorPin),_r2(r2)
        {
            pinMode(sensorPin, INPUT);
        };

        //
        // Reads the value of the temperature sensor, and returns in Celsius
        //
        float Read();

        //
        // Prints the temperature read to the serial montitor
        //
        void Print();

        //
        // Set voltage settings
        //
        void SetHardwareConfig(float vcc, float vccRes);

        //
        // Sets the Steinhart-Hart Coefficents.
        // To maintain the previous value, pass in -1
        //
        void SetSteinhartHartCoefficents(float a, float b, float c);
};