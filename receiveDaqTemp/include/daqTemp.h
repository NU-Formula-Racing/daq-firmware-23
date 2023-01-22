/**
 * @brief Definitions and constants for the DAQ temp board. Initializes the
 * ambient temperature sensor.
 *
 */
class TempBoard
{
public:
    TempBoard();

    // CAN address.
    const uint16_t kCANId{0x420};
};
