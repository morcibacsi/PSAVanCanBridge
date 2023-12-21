#include "MessageHandler_220.h"
#include "../Structs/CAN_220.h"

void MessageHandler_220::SetData()
{
    CAN_220_2004_Byte1Struct field1;
    field1.data.front_left_door_open  = _dataBroker->FrontLeftDoorOpen;
    field1.data.front_right_door_open = _dataBroker->FrontRightDoorOpen;

    field1.data.rear_left_door_open   = _dataBroker->RearLeftDoorOpen;
    field1.data.rear_right_door_open  = _dataBroker->RearRightDoorOpen;

    field1.data.hood_open  = _dataBroker->HoodOpen;
    field1.data.trunk_open = _dataBroker->TrunkOpen;

    field1.data.fuel_flap_open   = _dataBroker->FuelFlapOpen;
    field1.data.rear_window_open = _dataBroker->RearWindowOpen;

    _data[0] = field1.asByte;
}
