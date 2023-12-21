#include "MessageHandler_2B6.h"

uint8_t MessageHandler_2B6::SendMessage(unsigned long currentTime, bool forcedSend)
{
    _data[0] = _config->VIN_FOR_HEADUNIT[9];
    _data[1] = _config->VIN_FOR_HEADUNIT[10];
    _data[2] = _config->VIN_FOR_HEADUNIT[11];
    _data[3] = _config->VIN_FOR_HEADUNIT[12];
    _data[4] = _config->VIN_FOR_HEADUNIT[13];
    _data[5] = _config->VIN_FOR_HEADUNIT[14];
    _data[6] = _config->VIN_FOR_HEADUNIT[15];
    _data[7] = _config->VIN_FOR_HEADUNIT[16];

    return ICanMessageHandler::SendMessage(currentTime, forcedSend);
}