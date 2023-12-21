#include "MessageHandler_325.h"
#include "../../Helpers/IntUnions.h"

void MessageHandler_325::SetData(uint8_t cd1, uint8_t cd2, uint8_t error)
{
    _data[0] = cd1;
    _data[1] = cd2;
    _data[2] = error;
}

uint8_t MessageHandler_325::SendMessage(unsigned long currentTime, bool forcedSend)
{
    if (_config->BRIDGE_RADIO_DATA)
    {
        return ICanMessageHandler::SendMessage(currentTime, forcedSend);
    }

    return 0;
}