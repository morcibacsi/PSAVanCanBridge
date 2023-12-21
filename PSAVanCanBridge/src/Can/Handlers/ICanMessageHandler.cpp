#include "ICanMessageHandler.h"

ICanMessageHandler::ICanMessageHandler(ICanMessageSender *canInterface, DataBroker *dataBroker, uint16_t canId, uint8_t dataLength, uint16_t interval)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        _data[i] = 0;
    }

    _canInterface = canInterface;
    _dataBroker = dataBroker;
    _canId = canId;
    _dataLength = dataLength;
    _sendInterval = interval;
}

uint8_t ICanMessageHandler::SendMessage(unsigned long currentTime, bool forcedSend)
{
    if (forcedSend || currentTime - _previousSendTime > _sendInterval)
    {
        _previousSendTime = currentTime;
        return _canInterface->SendMessage(_canId, 0, _dataLength, _data);
    }
    return 0;
}

uint16_t ICanMessageHandler::GetCanId()
{
    return _canId;
}

void ICanMessageHandler::SetData()
{

}