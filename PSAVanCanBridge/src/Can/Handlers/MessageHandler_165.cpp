#include "MessageHandler_165.h"
#include "../Structs/CAN_165.h"

void MessageHandler_165::SetSource(uint8_t source)
{
    CAN_165_Byte3Struct field3;
    field3.data.source = source;
    _data[2] = field3.asByte;
}

void MessageHandler_165::SetData(uint8_t on, uint8_t muted)
{
    CAN_165_Byte1Struct field1;
    field1.data.power = on;
    field1.data.amplifier_on = on;
    field1.data.muted = muted;

    CAN_165_Byte2Struct field2;
    field2.data.has_tuner = 1;
    field2.data.has_cd = CAN_RADIO_HAS_CD_YES_OK;

    _data[0] = field1.asByte;
    _data[1] = field2.asByte;
}

uint8_t MessageHandler_165::SendMessage(unsigned long currentTime, bool forcedSend)
{
    if (_config->BRIDGE_RADIO_DATA)
    {
        return ICanMessageHandler::SendMessage(currentTime, forcedSend);
    }

    return 0;
}