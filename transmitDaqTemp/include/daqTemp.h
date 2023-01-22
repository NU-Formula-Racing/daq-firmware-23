/**
 * @brief Definitions and constants for the DAQ temp board. Initializes the
 * ambient temperature sensor.
 *
 */
class TempBoard
{
public:
    TempBoard();
    float ReadAmbientTempSensor();

    // CAN address.
    const uint16_t kCANId{0x420};

private:
    /// Pins
    static constexpr int temp_pin = A0;

    /// Scalars & Offsets
    static constexpr float VCC = 3.3;
    static constexpr float R2 = 10000;
    static constexpr float adc_resolution = 4095;

    static constexpr float A = 0.001129148;
    static constexpr float B = 0.000234125;
    static constexpr float C = 0.0000000846741;
};
