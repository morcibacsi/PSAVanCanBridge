#include "MessageHandler_161.h"

void MessageHandler_161::SetData()
{
    _data[2] = _dataBroker->OilTemperature;
    _data[3] = _dataBroker->FuelLevel;
    _data[6] = _dataBroker->OilLevel;
}