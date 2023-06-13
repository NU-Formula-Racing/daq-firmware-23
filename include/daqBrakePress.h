/**
 * @brief Definitions and constants for the DAQ brake pressure board. Read both the
 * front and rear break pressure sensors.
 *
 */
class BrakePressBoard
{
public:
    BrakePressBoard();

    float ReadBrakePressSensor();

    const uint16_t CANAddress{0x410};

private:

    /// Pins

    // Both front and rear break press sensors
    // have the same pin number.
    static constexpr int brakePressSensorPin = 36;

    // Brake Press Sensor
    // Direct scaling for break press sensor.
    static constexpr float brakePressScalar = 2.44;
};
