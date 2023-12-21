#include "MessageHandler_0F6.h"
#include "../Structs/CAN_0F6.h"

void MessageHandler_0F6::SetData()
{
    //{ 0x08, 0x28, 0x00, 0x00, 0x00, 0x00, 0x50, 0x00 }

    CanDash1Byte1Struct field1;
    field1.data.config_mode = _dataBroker->TrailerPresent == 1 ? 0 : 2;
    field1.data.ignition = _dataBroker->Ignition;

    field1.data.engine_status = _dataBroker->EngineRunning == 1 ? 2 : 0;
    field1.data.generator_status = _dataBroker->EngineRunning;

    CanDash1Byte8Struct field8;
    field8.data.reverse_gear_light = _dataBroker->IsReverseEngaged;
    field8.data.wiper_status = _dataBroker->WiperStatus;
    field8.data.turn_left_light = _dataBroker->LeftTurnIndicator;
    field8.data.turn_right_light = _dataBroker->RightTurnIndicator;

    _data[0] = field1.asByte;

    _data[1] = _dataBroker->CoolantTemperature;
    ///*
    _data[2] = _dataBroker->Odometer.data.leftByte;
    _data[3] = _dataBroker->Odometer.data.middleByte;
    _data[4] = _dataBroker->Odometer.data.rightByte;
    //*/
    _data[5] = _dataBroker->ExternalTemperature;
    _data[6] = _dataBroker->ExternalTemperature;
    _data[7] = field8.asByte;


    //_data[0] = 0x8E;
    /*
    _data[1] = 0x00;
    _data[2] = 0x00;
    _data[3] = 0x00;
    _data[4] = 0x00;
    _data[5] = 0x00;
    _data[6] = 0x00;
    _data[7] = 0x80;
    */
    //{ 0x8E, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x40, 0x80 };
}

