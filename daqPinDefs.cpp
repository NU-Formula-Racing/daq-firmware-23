/**
 * @brief Definitions and constants for the DAQ wheel board. Initializes with a wheel speed sensor pin, 
 * brake temp sensor pin, sus pos sensor pin, and can frame address.
 * 
 */
class WheelBoard
{
    public:
        int wheel_speed_sensor_pin;
        int brake_temp_sensor_pin;
        int suspension_position_sensor_pin;
        int can_frame_address;
        float brake_temp_scalar;
        float brake_temp_offset;
        float suspension_position_scalar;
        float suspension_position_offset;

        WheelBoard(int wss_pin, int bts_pin, int sps_pin, CANFrameAddress can_frame)
        {
            wheel_speed_sensor_pin = wss_pin;
            brake_temp_sensor_pin = bts_pin;
            suspension_position_sensor_pin = sps_pin;
            can_frame_address = can_frame;
            brake_temp_scalar = 1;
            brake_temp_offset = 0;
            suspension_position_scalar = 1;
            suspension_position_offset = 0;
        }
};

enum CANFrameAddress
{
    FL_CAN_FRAME_ADDRESS = 0x400,
    FR_CAN_FRAME_ADDRESS = 0x401,
    BL_CAN_FRAME_ADDRESS = 0x402,
    BR_CAN_FRAME_ADDRESS = 0x403
}