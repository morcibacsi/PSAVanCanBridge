#include "MessageHandler_350_2010.h"
#include "../Structs/CAN_350_2010.h"

void MessageHandler_350_2010::SetData()
{

    if (_config->SEND_AC_CHANGES_TO_DISPLAY)
    {
        CAN_350_2010Byte1Struct byte1;
        byte1.data.rear_window_heating_on = _dataBroker->IsWindowHeatingOn;
        byte1.data.ac_request = _dataBroker->IsAirConEnabled;
        byte1.data.visibility_request = _dataBroker->IsWindowHeatingOn; // maybe to display windshield icon?

        CAN_350_2010Byte6Struct byte6;
        byte6.data.air_inlet = (_dataBroker->IsRecyclingOn == 1) ? 3 : 0;

        if (_config->SEND_AC_FAN_CHANGES_TO_DISPLAY)
        {
            byte6.data.fan_speed = GetFanSpeed_2010(_dataBroker->FanSpeed);
        }

        _data[0] = byte1.asByte;
        _data[5] = byte6.asByte;
    }

}

uint8_t MessageHandler_350_2010::SendMessage(unsigned long currentTime, bool forcedSend)
{
    if (_config->SEND_AC_CHANGES_TO_DISPLAY)
    {
        return ICanMessageHandler::SendMessage(currentTime, forcedSend);
    }
    return 0;
}
