#include "VanRadioSettingsPacketSender.h"

VanRadioSettingsPacketSender::VanRadioSettingsPacketSender(IVanMessageSender * object)
{
    vanMessageSender = object;
}

bool VanRadioSettingsPacketSender::GetSettings(uint8_t channelId)
{
    return vanMessageSender->set_channel_for_reply_request_message(channelId, VAN_ID_RADIO_INFO, VAN_ID_RADIO_INFO_LENGTH, 1);
}

void VanRadioSettingsPacketSender::Disable(uint8_t channelId)
{
    vanMessageSender->disable_channel(channelId);
}
