#include "MessageHandler_3A7.h"
#include "../Structs/CAN_3A7.h"

void MessageHandler_3A7::SetData()
{
    if (_config->CAN_TYPE == 0)
    {
        Can3A7Byte2Struct maintenance;
        maintenance.data.maintenance_due = _dataBroker->IsMaintenanceDue;
        _data[1] = maintenance.asByte;
        _data[3] = _dataBroker->MaintenanceKilometers.data.leftByte;
        _data[4] = _dataBroker->MaintenanceKilometers.data.rightByte;
        _data[7] = 4;
    }
}

uint8_t MessageHandler_3A7::SendMessage(unsigned long currentTime, bool forcedSend)
{
    if (_config->CAN_TYPE == 0)
    {
        return ICanMessageHandler::SendMessage(currentTime, forcedSend);
    }

    return 0;
}