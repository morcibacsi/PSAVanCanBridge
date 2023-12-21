#include "MessageHandler_1D0.h"
#include "../Structs/CAN_1D0.h"

void MessageHandler_1D0::SetData()
{
    if (_config->SEND_AC_CHANGES_TO_DISPLAY)
    {
        CAN_1D0_Byte1Struct byte1;
        byte1.data.off = (_dataBroker->IsHeatingPanelOn == 1) ? 0 : 1;
        byte1.data.rear_window_heating_on = _dataBroker->IsWindowHeatingOn;
        byte1.data.ac_mode = (_dataBroker->IsHeatingPanelOn == 1) ? 0 : 1;

        _data[0] = byte1.asByte;
    }

    if (_config->SEND_AC_FAN_CHANGES_TO_DISPLAY)
    {
        CAN_1D0_Byte3Struct byte3;
        byte3.data.fan_speed = _dataBroker->FanSpeed;

        _data[2] = byte3.asByte;
    }
}

uint8_t MessageHandler_1D0::SendMessage(unsigned long currentTime, bool forcedSend)
{
    if (_config->SEND_AC_CHANGES_TO_DISPLAY)
    {
        return ICanMessageHandler::SendMessage(currentTime, forcedSend);
    }
    return 0;
}
