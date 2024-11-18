#include "VanRadioTunerPacketSender.h"
#include "../Structs/VAN_554.h"

VanRadioTunerPacketSender::VanRadioTunerPacketSender(IVanMessageSender * object)
{
    vanMessageSender = object;
}

bool VanRadioTunerPacketSender::GetTunerData(uint8_t channelId)
{
    vanMessageSender->set_channel_for_reply_request_message(channelId, VAN_ID_RADIO_TUNER, VAN_ID_RADIO_TUNER_LENGTH, 1);
    return true;
}

void VanRadioTunerPacketSender::Disable(uint8_t channelId)
{
    vanMessageSender->disable_channel(channelId);
}
