#include "MessageHandler_2A5.h"
#include "../../Helpers/IntUnions.h"

void MessageHandler_2A5::SetData(uint8_t letter1, uint8_t letter2, uint8_t letter3, uint8_t letter4, uint8_t letter5, uint8_t letter6, uint8_t letter7, uint8_t letter8)
{
    _data[0] = letter1;
    _data[1] = letter2;
    _data[2] = letter3;
    _data[3] = letter4;
    _data[4] = letter5;
    _data[5] = letter6;
    _data[6] = letter7;
    _data[7] = letter8;
}

uint8_t MessageHandler_2A5::SendMessage(unsigned long currentTime, bool forcedSend)
{
    if (_config->BRIDGE_RADIO_DATA)
    {
        return ICanMessageHandler::SendMessage(currentTime, forcedSend);
    }

    return 0;
}