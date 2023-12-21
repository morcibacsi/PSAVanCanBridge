#include "MessageHandler_2A1.h"

void MessageHandler_2A1::SetData()
{
    _data[0] = _dataBroker->Trip1Speed;
    _data[1] = _dataBroker->Trip1DistanceByte1;
    _data[2] = _dataBroker->Trip1DistanceByte2;
    _data[3] = _dataBroker->Trip1ConsumptionByte1;
    _data[4] = _dataBroker->Trip1ConsumptionByte2;
}
