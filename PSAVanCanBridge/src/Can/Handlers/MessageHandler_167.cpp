#include "MessageHandler_167.h"

uint8_t MessageHandler_167::SendMessage(unsigned long currentTime, bool forcedSend)
{
    if (_config->NO_EMF_C_INSTALLED)
    {
        return ICanMessageHandler::SendMessage(currentTime, forcedSend);
    }

    return 0;
}