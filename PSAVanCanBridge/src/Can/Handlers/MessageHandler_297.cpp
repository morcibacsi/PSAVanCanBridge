#include "MessageHandler_297.h"
#include "../Structs/CAN_297.h"

void MessageHandler_297::SetData()
{
    Can2004_297Byte1 status;
    status.data.is_active = _dataBroker->Ignition;

    _data[0] = status.asByte;
    _data[1] = _dataBroker->SpeedInKmh;
}
