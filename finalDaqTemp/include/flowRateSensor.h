/* flowRateSensor.h*/

#include <Arduino.h>

// * I realize that this abstraction is very overkill, but this is more for the sake that we are working as a team
// * This, in some circles, is considered best practice; it is best to ensure that our code follows best practices for the sake of following years

//
// The FlowRateSensor is an abstraction of a Hall Effect based Flow Rate Sensor. For details, see: https://how2electronics.com/arduino-water-flow-sensor-measure-flow-rate-volume/
// The user should must know the pin of the sensor's signal wire, which is passed through the constructor
//
class FlowRateSensor
{
    private:
        int _sensorPin;


    public:

        //
        // Create a FlowRateSensor object who's signal wire is at "sensorPin"
        //
        FlowRateSensor(int sensorPin):
            _sensorPin(sensorPin){}

        //
        // Reads the flow rate from the sensor, and returns it in liters/minute
        // Note, this function will halt the execution of the program until a pulse from the sensor is read
        // Normally, this delay is insignificant, but it should be noted.
        //
        float Read();

        //
        // Prints the value of thhe temperature sensor
        //
        void Print() const;
};