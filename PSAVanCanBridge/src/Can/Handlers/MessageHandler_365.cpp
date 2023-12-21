#include "MessageHandler_365.h"

void MessageHandler_365::SetData(uint8_t number_of_tracks, uint8_t total_minutes, uint8_t total_seconds)
{
    _data[0] = number_of_tracks;
    _data[1] = total_minutes;
    _data[2] = total_seconds;
}

uint8_t MessageHandler_365::SendMessage(unsigned long currentTime, bool forcedSend)
{
    if (_config->BRIDGE_RADIO_DATA)
    {
        return ICanMessageHandler::SendMessage(currentTime, forcedSend);
    }

    return 0;
}