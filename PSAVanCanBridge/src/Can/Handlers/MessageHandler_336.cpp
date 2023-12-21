#include "MessageHandler_336.h"

uint8_t MessageHandler_336::SendMessage(unsigned long currentTime, bool forcedSend)
{
    _data[0] = _config->VIN_FOR_HEADUNIT[0];
    _data[1] = _config->VIN_FOR_HEADUNIT[1];
    _data[2] = _config->VIN_FOR_HEADUNIT[2];

    return ICanMessageHandler::SendMessage(currentTime, forcedSend);
}