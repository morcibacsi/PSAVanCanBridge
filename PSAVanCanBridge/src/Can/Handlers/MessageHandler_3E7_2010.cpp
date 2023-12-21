#include "MessageHandler_3E7_2010.h"
#include "../Structs/CAN_3E7_2010.h"

void MessageHandler_3E7_2010::SetData()
{
    CAN_3E7_2010Byte1Struct byte1;
    byte1.data.wrench_icon = _dataBroker->IsMaintenanceDue;
    _data[1] = byte1.asByte;
    _data[2] = 4;
    _data[3] = _dataBroker->MaintenanceKilometers.data.leftByte;
    _data[4] = _dataBroker->MaintenanceKilometers.data.rightByte;
}

uint8_t MessageHandler_3E7_2010::SendMessage(unsigned long currentTime, bool forcedSend)
{
    return ICanMessageHandler::SendMessage(currentTime, forcedSend);
}