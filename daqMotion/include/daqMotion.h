/**
 * @brief Definitions and constants for the DAQ wheel board. Initializes with a wheel speed sensor pin, 
 * brake temp sensor pin, sus pos sensor pin, and can frame address.
 * 
 */
class MotionBoard
{
    public:
        //constructor
        MotionBoard();
        //Sensor Readings: : 
        //accelerometer 
        float ReadXAccel();
        float ReadYAccel();
        float ReadZAccel();
        //gyroscope
        float ReadXGyro();
        float ReadYGyro();
        float ReadZGyro();
        //GPS
        float ReadLat();
        float ReadLon();
        //setup
        void setupMotionBoard();
    private:
        // pins : TODO 

        static constexpr int brakeTempSensorPin = 2;



        static constexpr float brakeTempScalar = 1;
        static constexpr float brakeTempOffset = 0;
        // wheel speed sensor
        unsigned long current_pulse_time;
        unsigned long previous_pulse_time;
        unsigned long pulse_duration;

        // imu and gps 
};