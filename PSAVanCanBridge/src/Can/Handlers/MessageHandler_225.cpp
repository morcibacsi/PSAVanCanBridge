#include "MessageHandler_225.h"
#include "../Structs/CAN_225.h"

void MessageHandler_225::SetData(uint8_t frequency_scan_status, uint8_t tuner_sensitivity, uint8_t memo_num, uint8_t band, uint8_t freq1, uint8_t freq2)
{
    CAN_225_Byte1Struct field1;
    field1.data.frequency_scan_direction = frequency_scan_status;
    field1.data.tuner_sensitivity = tuner_sensitivity;

    CAN_225_Byte2Struct field2;
    field2.data.memory_num = memo_num;

    CAN_225_Byte3Struct field3;
    field3.data.band_type = band;

    _data[0] = field1.asByte;
    _data[1] = field2.asByte;
    _data[2] = field3.asByte;

    _data[3] = freq1;
    _data[4] = freq2;
}

uint8_t MessageHandler_225::SendMessage(unsigned long currentTime, bool forcedSend)
{
    if (_config->BRIDGE_RADIO_DATA)
    {
        return ICanMessageHandler::SendMessage(currentTime, forcedSend);
    }

    return 0;
}