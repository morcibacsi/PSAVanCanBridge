#include "MessageHandler_036.h"
#include "../Structs/CAN_036.h"
#include "../Structs/CAN_036_2010.h"

void MessageHandler_036::SetData()
{
    if (_config->CAN_TYPE == 0)
    {
        SetData2004();
    }
    else
    {
        SetData2010();
    }
}

void MessageHandler_036::SetData2004()
{
    CanIgnitionByte3Struct ecoField;
    ecoField.data.economy_mode_active = _dataBroker->EconomyMode;

    CanIgnitionByte4Struct brightnessField;
    brightnessField.data.night_mode = _dataBroker->NightMode;
    brightnessField.data.dashboard_brightness = _dataBroker->Brightness;
    brightnessField.data.black_panel_status = _dataBroker->BlackPanel;

    CanIgnitionByte5Struct ignitionField;

    CanIgnitionByte7Struct rearCamField;

    if (_config->RADIO_TYPE == 5 && _config->ENABLE_REVERSE_CAMERA_ON_RTX)
    {
        rearCamField.data.activate_rear_camera = _dataBroker->IsReverseEngaged;
    }

    //ignitionField.data.ignition_mode = _dataBroker->Ignition;
    ///*
    if (_dataBroker->StandBySoon)
    {
        ignitionField.data.ignition_mode = CAN_IGNITION_MODE_STANDBY_SOON;
    }
    else
    {
        if (_dataBroker->Ignition)
        {
            ignitionField.data.ignition_mode = CAN_IGNITION_MODE_NORMAL;
        }
        else
        {
            ignitionField.data.ignition_mode = CAN_IGNITION_MODE_STANDBY;
        }
    }
    //*/
    _data[2] = ecoField.asByte;
    _data[3] = brightnessField.asByte;
    _data[4] = ignitionField.asByte;
    _data[6] = rearCamField.asByte;
    _data[7] = 0xA0;
}

void MessageHandler_036::SetData2010()
{
    CAN_036_2010_Byte1Struct byte1;
    byte1.data.driving_direction = _dataBroker->IsReverseEngaged ? DRIVING_DIRECTION_REVERSE : DRIVING_DIRECTION_FORWARD;

    CAN_036_2010_Byte3Struct byte3;
    byte3.data.economy_mode_active = _dataBroker->EconomyMode;

    CAN_036_2010_Byte4Struct byte4;
    byte4.data.night_mode = _dataBroker->NightMode;
    byte4.data.dashboard_brightness = _dataBroker->Brightness;
    byte4.data.black_panel_status = _dataBroker->BlackPanel;

    CAN_036_2010_Byte5Struct byte5;
    if (_dataBroker->StandBySoon)
    {
        byte5.data.ignition_mode = CAN_2010_IGNITION_MODE_STANDBY_SOON;
    }
    else
    {
        if (_dataBroker->Ignition)
        {
            byte5.data.ignition_mode = CAN_2010_IGNITION_MODE_NORMAL;
        }
        else
        {
            byte5.data.ignition_mode = CAN_2010_IGNITION_MODE_STANDBY;
        }
    }
    _data[0] = byte1.asByte;
    _data[2] = byte3.asByte;
    _data[3] = byte4.asByte;
    _data[4] = byte5.asByte;
    _data[7] = 0xA0;
}