#include "MessageHandler_2E1.h"
#include "../Structs/CAN_2E1_2004.h"

void MessageHandler_2E1::SetData()
{
    CAN_2E1_2004_Byte1Struct field1;
    field1.data.automatic_door_locking_status = _dataBroker->DeadlockActive ? AUTO_DOOR_LOCKING_STATUS_ACTIVATED : AUTO_DOOR_LOCKING_STATUS_NOT_ACTIVATED;
    field1.data.auto_headlight_status = _dataBroker->AutoHeadlampActive ? AUTO_HEADLIGHT_STATUS_ACTIVATED : AUTO_HEADLIGHT_STATUS_NOT_ACTIVATED;
    field1.data.passenger_airbag_status = _dataBroker->PassengerAirbag ? PASSENGER_AIRBAG_STATUS_NOT_ACTIVATED : PASSENGER_AIRBAG_STATUS_ACTIVATED;

    CAN_2E1_2004_Byte2Struct field2;
    field2.data.auto_wiper_status = _dataBroker->AutoWipingActive ? AUTO_WIPER_STATUS_ACTIVATED : AUTO_WIPER_STATUS_NOT_ACTIVATED;
    field2.data.esp_status = _dataBroker->EspDeactivated ? ESP_STATUS_NOT_ACTIVATED : ESP_STATUS_ACTIVATED;

    CAN_2E1_2004_Byte3Struct field3;
    field3.data.child_safety_status = _dataBroker->PassengerAirbag ? CHILD_SAFETY_STATUS_ACTIVATED : CHILD_SAFETY_STATUS_NOT_ACTIVATED;

    _data[0] = field1.asByte;
    _data[1] = field2.asByte;
    _data[2] = field3.asByte;
}
