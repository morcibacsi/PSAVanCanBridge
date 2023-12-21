#include "VanRadioCommandPacketSender.h"
#include <Arduino.h>
#include "../Structs/VAN_8D4.h"

VanRadioCommandPacketSender::VanRadioCommandPacketSender(IVanMessageSender * object, uint8_t channelId)
{
    vanMessageSender = object;
    _channelId = channelId;
}

bool VanRadioCommandPacketSender::SetRadioState(uint8_t powerOn, uint8_t active, uint8_t keyboardEnabled, uint8_t autoVol, uint8_t loudness, uint8_t mute)
{
    VAN_RAD_CMD_GEN_Byte1Struct byte1;
    byte1.data.auto_volume_on = autoVol;
    byte1.data.loudness_on = loudness;
    byte1.data.mute = mute;
    byte1.data.power_on = powerOn;
    byte1.data.active_on = active;
    byte1.data.button_mode = keyboardEnabled;

    _commandState = byte1.asByte;

    uint8_t data1[] = { VAN_RADIO_COMMAND_GEN_CMD_ID_GEN, byte1.asByte };
    return vanMessageSender->set_channel_for_transmit_message(_channelId, 0x8D4, data1, 2, 1);
}

bool VanRadioCommandPacketSender::SendSource(uint8_t source)
{
    VAN_RAD_CMD_SRC_Byte1Struct byte1;
    byte1.data.source = source;

    uint8_t data2[] = { VAN_RADIO_COMMAND_GEN_CMD_ID_SRC, byte1.asByte };
    return vanMessageSender->set_channel_for_transmit_message(_channelId, 0x8D4, data2, 2, 1);
}

bool VanRadioCommandPacketSender::SetKeyboardState(uint8_t enabled)
{
    VAN_RAD_CMD_GEN_Byte1Struct byte1;
    byte1.asByte = _commandState;

    if (byte1.data.active_on == 1 && byte1.data.power_on == 1)
    {
        byte1.data.button_mode = enabled;
    }

    uint8_t data[] = { VAN_RADIO_COMMAND_GEN_CMD_ID_GEN, byte1.asByte };
    return vanMessageSender->set_channel_for_transmit_message(_channelId, 0x8D4, data, 2, 1);
}

bool VanRadioCommandPacketSender::SetAudioSettings(uint8_t exitOptions, uint8_t balance, uint8_t fader, uint8_t bass, uint8_t treble)
{
    VAN_RAD_CMD_AUD_Byte2Struct balanceByte;
    balanceByte.data.balance_value = balance;
    balanceByte.data.exit_from_settings_mode = exitOptions;

    VAN_RAD_CMD_AUD_ByteStruct faderByte;
    faderByte.data.value = fader;

    VAN_RAD_CMD_AUD_ByteStruct bassByte;
    bassByte.data.value = bass;

    VAN_RAD_CMD_AUD_ByteStruct trebleByte;
    trebleByte.data.value = treble;

    uint8_t data[] = { VAN_RADIO_COMMAND_GEN_CMD_ID_AUD, balanceByte.asByte, faderByte.asByte, bassByte.asByte, trebleByte.asByte };
    return vanMessageSender->set_channel_for_transmit_message(_channelId, 0x8D4, data, 5, 1);
}

void VanRadioCommandPacketSender::Disable()
{
    vanMessageSender->disable_channel(_channelId);
}
