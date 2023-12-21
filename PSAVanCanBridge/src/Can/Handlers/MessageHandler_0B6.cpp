#include "MessageHandler_0B6.h"

void MessageHandler_0B6::SetData()
{
    _data[0] = _dataBroker->Rpm.data.leftByte;
    _data[1] = _dataBroker->Rpm.data.rightByte;
    _data[2] = _dataBroker->Speed.data.leftByte;
    _data[3] = _dataBroker->Speed.data.rightByte;
    _data[4] = _dataBroker->DistanceForCMB1;
    _data[5] = _dataBroker->DistanceForCMB2;
    _data[6] = _dataBroker->Consumption;
    _data[7] = 0xD0;
}
