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
        //Sensor Readings 
        float* ReadGPS();
        float* ReadAccel();
        float* ReadGyro();
        //setup
        void setupMotionBoard();
    private:
        // pins : TODO 

        static constexpr int brakeTempSensorPin = 2;
        // scalars and offsets
        static constexpr float brakeTempScalar = 1;
        static constexpr float brakeTempOffset = 0;
        // wheel speed sensor
        unsigned long current_pulse_time;
        unsigned long previous_pulse_time;
        unsigned long pulse_duration;

        // imu and gps 
};