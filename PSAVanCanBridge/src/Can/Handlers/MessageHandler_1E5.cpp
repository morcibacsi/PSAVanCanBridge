#include "MessageHandler_1E5.h"
#include "../Structs/CAN_1E5.h"

void MessageHandler_1E5::SetData(uint8_t balance, uint8_t fader, uint8_t bass, uint8_t treble, uint8_t show_loudness, uint8_t loudness_on, uint8_t show_auto_vol, uint8_t auto_vol_on)
{
    CAN_1E5_Byte6Struct field6;
    field6.data.loudness_on = loudness_on;
    field6.data.show_loudness = show_loudness;

    field6.data.auto_vol = (auto_vol_on == 0) ? CAN_AUDIO_SETTINGS_AUTO_VOL_OFF : CAN_AUDIO_SETTINGS_AUTO_VOL_ON;
    field6.data.show_auto_vol = show_auto_vol;

    _data[0] = balance;
    _data[1] = fader;
    _data[2] = bass;
    _data[4] = treble;
    _data[5] = field6.asByte;
}

uint8_t MessageHandler_1E5::SendMessage(unsigned long currentTime, bool forcedSend)
{
    if (_config->BRIDGE_RADIO_DATA)
    {
        return ICanMessageHandler::SendMessage(currentTime, forcedSend);
    }
    return 0;
}