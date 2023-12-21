#include "MessageHandler_10B.h"

void MessageHandler_10B::SetData()
{
    _data[0] = _dataBroker->SteeringAngleByte1;
    _data[1] = _dataBroker->SteeringAngleByte2;
    _data[2] = _dataBroker->SteeringSpeedByte;
    _data[3] = _dataBroker->SteeringStatusByte;
}
