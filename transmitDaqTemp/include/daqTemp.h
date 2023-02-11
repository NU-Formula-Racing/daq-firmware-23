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
    static constexpr int temp_pin = A0;
    static constexpr int flow_pin = A7;

    // Coolant Flow Count Value
    unsigned int flowCount;

private:
    /// Scalars & Offsets

    // General Constants
    static constexpr float VCC = 3.3;
    static constexpr float adc_resolution = 4095;

    // Temp Constants
    // static constexpr float R2 = 10000;
    // static constexpr float A = 0.001129148;
    // static constexpr float B = 0.000234125;
    // static constexpr float C = 0.0000000846741;
    const float R2 = 10000;
    const float A = 0.00086464570507481025671920266089667116571417420815226;
    const float B = 0.00025469298487325369343931097410154953511601128782288;
    const float C = 0.00000016939334252071086626365492600511414247094647827816;
};
