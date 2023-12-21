#include "MessageHandler_261.h"

void MessageHandler_261::SetData()
{
    _data[0] = _dataBroker->AverageSpeed;
    _data[1] = _dataBroker->DistanceByte1;
    _data[2] = _dataBroker->DistanceByte2;
    _data[3] = _dataBroker->ConsumptionByte1;
    _data[4] = _dataBroker->ConsumptionByte2;
}
