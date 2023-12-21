#include "MessageHandler_3A5.h"
#include "../../Helpers/IntUnions.h"

void MessageHandler_3A5::SetData(uint8_t number_of_current_track, uint8_t total_track_minutes, uint8_t total_track_seconds, uint8_t elapsed_track_minutes, uint8_t elapsed_track_seconds)
{
    _data[0] = number_of_current_track;
    _data[1] = total_track_minutes;
    _data[2] = total_track_seconds;
    _data[3] = elapsed_track_minutes;
    _data[4] = elapsed_track_seconds;
}

uint8_t MessageHandler_3A5::SendMessage(unsigned long currentTime, bool forcedSend)
{
    if (_config->BRIDGE_RADIO_DATA)
    {
        return ICanMessageHandler::SendMessage(currentTime, forcedSend);
    }

    return 0;
}