#pragma once
#include "can_interface.h"
#include "virtualTimer.h"

class DaqTemp
{
public:
    DaqTemp(ICAN &can_interface) : can_interface_{can_interface} {}

    float GetMotorTemp()
    {
        return motor_temp;
    }

    float GetCoolantTemp()
    {
        return coolant_temp;
    }

    float GetAmbientTemp()
    {
        return ambient_temp;
        ;
    }

private:
    ICAN &can_interface_;

    CANSignal<float, 0, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(-40), false> motor_temp{};
    CANSignal<float, 16, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(-40), false> coolant_temp{};
    CANSignal<float, 32, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(-40), false> ambient_temp{};
    CANRXMessage<3> rx_message{can_interface_, 0x420, motor_temp, coolant_temp, ambient_temp};
};