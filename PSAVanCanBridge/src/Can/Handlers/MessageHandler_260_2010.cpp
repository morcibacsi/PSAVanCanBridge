#include "MessageHandler_260_2010.h"
#include "../Structs/CAN_260_2010.h"

void MessageHandler_260_2010::SetData()
{
    if (_config->CAN_TYPE == 1)
    {
        CAN_260_2010_Byte1Struct byte1;
        byte1.asByte = 0;
        byte1.data.consumption_unit = 0;
        byte1.data.distance_unit    = 0;
        byte1.data.language         = LANGUAGE_ENGLISH;
        byte1.data.unit_and_language_data_valid = 1;

        CAN_260_2010_Byte2Struct byte2;
        byte2.asByte = 0;
        byte2.data.volume_unit      = 0;
        byte2.data.temperature_unit = 0;
        byte2.data.ambience_level   = AMBIENCE_LEVEL_6;
        byte2.data.sound_harmony    = SOUND_HARMONY_1;
        byte2.data.vehicle_function_data = 1;

        _data[0] = byte1.asByte;
        _data[1] = byte2.asByte;
        _data[2] = 0x00;
        _data[3] = 0x00;
        _data[4] = 0x00;
        _data[5] = 0x00;
        _data[6] = 0x00;
        _data[7] = 0x00;
    }
}

uint8_t MessageHandler_260_2010::SendMessage(unsigned long currentTime, bool forcedSend)
{
    if (_config->CAN_TYPE == 1)
    {
        return ICanMessageHandler::SendMessage(currentTime, forcedSend);
    }

    return 0;
}