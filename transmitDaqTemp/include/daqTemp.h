/**
 * @brief Definitions and constants for the DAQ temp board. Initializes the
 * ambient temperature sensor.
 *
 */
class TempBoard
{
public:
    // Constructor
    TempBoard();

    // Methods
    float ReadAmbientTempSensor();
    float ReadCoolantFlowRate();

    // CAN address.
    const uint16_t kCANId1{0x420};
    const uint16_t kCANId2{0x421};

    // enum class kCANID{
    //     PTrain = 0x420,
    //     Coolant = 0x421
    // };

    /// Pins
    static constexpr int temp_pin = 32;
    static constexpr int flow_pin = 34;

    // Coolant Flow Count Value
    unsigned int flowCount;

private:
    /// Scalars & Offsets

    // General Constants
    static constexpr float VCC = 3.3;
    static constexpr float adc_resolution = 4095;

    // Temp Constants
    static constexpr float R2 = 10000;
    static constexpr float A = 0.0008646457;
    static constexpr float B = 0.0002546929;
    static constexpr float C = 0.0000001693933;
};
