/**
 * @brief Definitions and constants for the DAQ brake pressure board. Read both the
 * front and rear break pressure sensors.
 *
 */
class BrakeBoard
{

public:
    BrakeBoard();

    float ReadBrakePressSensor();

    // Front and rear CAN addresses.
    const uint16_t kCANFRONT{0x410};
    const uint16_t kCANREAR{0x411};

private:
    /// Pins
    // Both front and rear break press sensors
    // have the same pin number.
    static constexpr int BrakePressSensorPin = 0;

    /// Scalars & Offsets

    // Brake Press Sensor
    // Direct scaling for break press sensor.
    static constexpr float brakePressScalar = 2.44;
};
