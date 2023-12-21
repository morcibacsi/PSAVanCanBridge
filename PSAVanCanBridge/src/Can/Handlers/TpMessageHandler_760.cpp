#include "TpMessageHandler_760.h"
#include "../../Can/Structs/CAN_760_RD4_RD43.h"
#include "../../Helpers/DebugPrint.h"

void TpMessageHandler_760::InternalProcess()
{
    //this keeps the diag mode alive

    /*
    // TODO this should be using the send_sf or the Send method somehow
    uint8_t data[] = { 0x01, 0x3E };
    _canInterface->SendMessage(0x760, 0, 2, data);
    */
}

void TpMessageHandler_760::ReceiveFinished()
{
    // Entered diag mode
    if (_rxMsg.Buffer[0] == 0x50 && _rxMsg.Buffer[1] == 0xC0)
    {
        GetVinNumber();
    }

    // Finished reading  VIN number
    if (_rxMsg.Buffer[0] == 0x61 && _rxMsg.Buffer[1] == _radioType)
    {
        memcpy(VinNumber, _rxMsg.Buffer+2, VIN_LENGTH);
        memcpy(_config->VIN_FOR_HEADUNIT, _rxMsg.Buffer+2, VIN_LENGTH);

        if (_vinFlashStorage->Save())
        {
            debug_println(F("VIN number saved"));
        }
        else
        {
            debug_println(F("VIN number not saved"));
        }
    }

    // Finished reading Options
    if (_rxMsg.Buffer[0] == 0x61 && _rxMsg.Buffer[1] == 0xC0)
    {
        CAN_760_RD4_RD43_OptionsStruct options;
        memcpy(&options, _rxMsg.Buffer+2, sizeof(options));

        debug_print("options.Field1.data.cd_changer_or_usb_enabled: "),debug_println(options.Field1.data.cd_changer_or_usb_enabled);
        debug_print("options.Field2.data.amplifier_enabled: "),debug_println(options.Field2.data.amplifier_enabled);
        debug_print("options.Field4.data.is_amplifier_multiplexed: "),debug_println(options.Field4.data.is_amplifier_multiplexed);

        debug_print("Field1: "),debug_println(options.Field1.asByte, HEX);
        debug_print("Field2: "),debug_println(options.Field2.asByte, HEX);
        debug_print("Field3: "),debug_println(options.Field3.asByte, HEX);
        debug_print("Field4: "),debug_println(options.Field4.asByte, HEX);
        debug_print("Field5: "),debug_println(options.Field5.asByte, HEX);
    }
//
/*
    for (uint8_t i = 0; i < _rxMsg.len; i++)
    {
        debug_print(_rxMsg.Buffer[i], HEX);debug_print(' ');
    }
    debug_println();
//*/
}

void TpMessageHandler_760::EnterDiagMode()
{
    uint8_t data[] = { 0x10, 0xC0 };
    Send(data, 2);
}

void TpMessageHandler_760::KeepAlive()
{
    uint8_t data[] = { 0x3E };
    Send(data, 1);
}

void TpMessageHandler_760::ClearFaults()
{
    uint8_t data[] = { 0x14, 0xFF, 0x00 };
    Send(data, 3);
}

void TpMessageHandler_760::GetBrightnessLevels()
{
    uint8_t data[] = { MODE_READ, 0xB0 };
    Send(data, 2);
}

void TpMessageHandler_760::GetVinNumber()
{
    // RD4, RD43: 2nd byte: 0xCC
    // RD45     : 2nd byte: 0xB0
    //uint8_t _radioType = 0xCC;
    uint8_t data[] = { MODE_READ, _radioType };
    Send(data, 2);
}

void TpMessageHandler_760::SetVinNumber(uint8_t *vinNumber)
{
    // RD4, RD43: 2nd byte: 0xCC
    // RD45     : 2nd byte: 0xB0
    //uint8_t _radioType = 0xCC;
    uint8_t data[19];

    data[0] = MODE_WRITE;
    data[1] = _radioType;

    for (size_t i = 0; i < 17; i++)
    {
        data[i+2] = vinNumber[i];
    }

    Send(data, 19);
}

void TpMessageHandler_760::GetOptions()
{
    uint8_t data[] = { MODE_READ, ADDRESS_OPTIONS };
    Send(data, 2);
}

void TpMessageHandler_760::SetOptions(uint8_t *options)
{
    uint8_t data[7];

    data[0] = MODE_WRITE;
    data[1] = ADDRESS_OPTIONS;

    for (size_t i = 0; i < 5; i++)
    {
        data[i+2] = options[i];
    }

    Send(data, 7);
}

void TpMessageHandler_760::ExitDiagMode()
{
    uint8_t data[] = { 0x10, 0x81 };
    Send(data, 2);
}

void TpMessageHandler_760::SetRadioType(uint8_t radioType)
{
    _radioType = radioType;
}