#include "MessageHandler_228_2010.h"
#include "../Structs/CAN_228_2010.h"

void MessageHandler_228_2010::SetData()
{
    if (_config->CAN_TYPE == 1)
    {
        CAN_228_2010_Byte3Struct byte2;
        byte2.data.activate_function = _dataBroker->CruiseControlActivateFunction;
        byte2.data.selected_function = _dataBroker->CruiseControlSelectedFunction;
        byte2.data.status_of_selected_function = _dataBroker->CruiseControlStatusOfSelectedFunction;
        byte2.data.setting_status = 1;
        byte2.data.target_present = 1;

        _data[0] = _dataBroker->CruiseControlSpeed.data.leftByte;
        _data[1] = _dataBroker->CruiseControlSpeed.data.rightByte;
        _data[2] = byte2.asByte;
        _data[3] = 0x80;
        _data[4] = 0x14;
        _data[5] = 0x7F;
        _data[6] = 0xFF;
        _data[7] = 0x98;
    }
}

uint8_t MessageHandler_228_2010::SendMessage(unsigned long currentTime, bool forcedSend)
{
    if (_config->CAN_TYPE == 1)
    {
        return ICanMessageHandler::SendMessage(currentTime, forcedSend);
    }

    return 0;
}