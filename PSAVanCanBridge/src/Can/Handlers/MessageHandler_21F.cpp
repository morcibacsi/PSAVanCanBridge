#include "MessageHandler_21F.h"

void MessageHandler_21F::SetData()
{
    _data[0] = _dataBroker->RadioRemoteButton;
    _data[1] = _dataBroker->RadioRemoteScroll;
}

uint8_t MessageHandler_21F::SendMessage(unsigned long currentTime, bool forcedSend)
{
    if (_config->BRIDGE_REMOTE_STALK_DATA)
    {
        return ICanMessageHandler::SendMessage(currentTime, forcedSend);
    }

    return 0;
}