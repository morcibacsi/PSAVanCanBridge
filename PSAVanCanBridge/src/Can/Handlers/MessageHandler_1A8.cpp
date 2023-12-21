#include "MessageHandler_1A8.h"
#include "../Structs/CAN_1A8_2004.h"

void MessageHandler_1A8::SetData()
{
    if (_config->CAN_TYPE == 0)
    {
        _data[0] = _dataBroker->CruiseControlStatus;
        _data[1] = _dataBroker->CruiseControlSpeed.data.leftByte;
        _data[2] = _dataBroker->CruiseControlSpeed.data.rightByte;
    }

    _data[5] = _dataBroker->TripOnCMB.data.leftByte;
    _data[6] = _dataBroker->TripOnCMB.data.middleByte;
    _data[7] = _dataBroker->TripOnCMB.data.rightByte;
}

/*
void MessageHandler_1A8::SendMessage(unsigned long currentTime, bool forcedSend)
{
    //if (_config->CAN_TYPE == 0)
    {
        ICanMessageHandler::SendMessage(currentTime, forcedSend);
    }

}
*/