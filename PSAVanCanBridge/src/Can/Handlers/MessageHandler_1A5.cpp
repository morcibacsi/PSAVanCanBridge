#include "MessageHandler_1A5.h"
#include "../Structs/CAN_1A5.h"

void MessageHandler_1A5::SetData(uint8_t origin, uint8_t volume)
{
    CAN_1A5_Byte1Struct field1;
    field1.data.level = volume;

    if (origin == 1){
        field1.data.origin = CAN_RADIO_VOLUME_CHANGE_USER;
    }
    else
    {
        field1.data.origin = CAN_RADIO_VOLUME_CHANGE_OTHER;
    }

    _data[0] = field1.asByte;
}

uint8_t MessageHandler_1A5::SendMessage(unsigned long currentTime, bool forcedSend)
{
    if (_config->BRIDGE_RADIO_DATA)
    {
        return ICanMessageHandler::SendMessage(currentTime, forcedSend);
    }
    return 0;
}