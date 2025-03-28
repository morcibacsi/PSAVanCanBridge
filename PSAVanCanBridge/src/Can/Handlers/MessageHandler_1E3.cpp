#include "MessageHandler_1E3.h"
#include "../Structs/CAN_1E3.h"

void MessageHandler_1E3::SetData()
{
    if (_config->SEND_AC_CHANGES_TO_DISPLAY)
    {
        CanAirConOnDisplayByte1Struct byte1;
        //byte1.data.ac_compressor_off = _dataBroker->IsACCompressorOn == 1 ? 0 : 1;
        byte1.data.ac_compressor_off = (_dataBroker->IsAirConEnabled == 1) ? 0 : 1;
        //byte1.data.off = (_dataBroker->Ignition == 1 && _dataBroker->IsHeatingPanelOn == 1) ? 0 : 1;
        byte1.data.off = (_dataBroker->IsHeatingPanelOn == 1) ? 0 : 1;// (_dataBroker->Ignition == 1 && _dataBroker->IsHeatingPanelOn == 1) ? 0 : 1;
        byte1.data.recycling_on = _dataBroker->IsRecyclingOn;
        byte1.data.recycling_on_pushed = _dataBroker->IsRecyclingOn;

        CanAirConOnDisplayByte2Struct byte2;
        byte2.data.rear_window_heating_on = _dataBroker->IsWindowHeatingOn;
        byte2.data.windshield = _dataBroker->IsWindowHeatingOn;

        _data[0] = byte1.asByte;
        _data[1] = byte2.asByte;
    }

    if (_config->QUERY_AC_STATUS)
    {
        _data[4] = _dataBroker->AirConDirection;
        _data[5] = _dataBroker->AirConDirection;
    }

    if (_config->SEND_AC_FAN_CHANGES_TO_DISPLAY)
    {
        _data[6] = _dataBroker->FanSpeed;
    }
}

uint8_t MessageHandler_1E3::SendMessage(unsigned long currentTime, bool forcedSend)
{
    if (_config->SEND_AC_CHANGES_TO_DISPLAY)
    {
        return ICanMessageHandler::SendMessage(currentTime, forcedSend);
    }
    return 0;
}
