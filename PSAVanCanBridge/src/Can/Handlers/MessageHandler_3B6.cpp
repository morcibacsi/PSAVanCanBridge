#include "MessageHandler_3B6.h"

uint8_t MessageHandler_3B6::SendMessage(unsigned long currentTime, bool forcedSend)
{
    _data[0] = _config->VIN_FOR_HEADUNIT[3];
    _data[1] = _config->VIN_FOR_HEADUNIT[4];
    _data[2] = _config->VIN_FOR_HEADUNIT[5];
    _data[3] = _config->VIN_FOR_HEADUNIT[6];
    _data[4] = _config->VIN_FOR_HEADUNIT[7];
    _data[5] = _config->VIN_FOR_HEADUNIT[8];

    return ICanMessageHandler::SendMessage(currentTime, forcedSend);
}