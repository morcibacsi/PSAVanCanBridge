#include "MessageHandler_0E6.h"
#include "../Structs/CAN_0E6.h"

uint8_t MessageHandler_0E6::chk_esp(uint8_t* buf){
    static uint8_t cntr = 0;

    uint8_t sum = cntr;
    for (uint8_t i = 0; i < 7; i++){
        sum += buf[i] >> 4;
        sum += buf[i] & 0x0F;
    }

    sum = (cntr << 4) | ((0x7FFC - sum) & 0x0F);
    cntr = cntr < 0x0F ? cntr + 1 : 0;

    return sum;
}

void MessageHandler_0E6::SetData()
{
    _data[1] = _dataBroker->RearLeftWheelCounter1;
    _data[2] = _dataBroker->RearLeftWheelCounter2;
    _data[3] = _dataBroker->RearRightWheelCounter1;
    _data[4] = _dataBroker->RearRightWheelCounter2;
}

uint8_t MessageHandler_0E6::SendMessage(unsigned long currentTime, bool forcedSend)
{
    if (_config->CAN_TYPE == 1)
    {
        _dataLength = 8;
        _data[5] = 0x83;
        _data[6] = 0x8C;
        _data[7] = chk_esp(_data);
    }
    return ICanMessageHandler::SendMessage(currentTime, forcedSend);
}