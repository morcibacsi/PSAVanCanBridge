#include "VanCarStatusPacketSender.h"
#include "../Structs/VAN_564.h"

VanCarStatusPacketSender::VanCarStatusPacketSender(IVanMessageSender* object)
{
    vanMessageSender = object;
}

bool VanCarStatusPacketSender::GetCarStatus(uint8_t channelId)
{
    return vanMessageSender->set_channel_for_reply_request_message(channelId, VAN_ID_CARSTATUS, 29, 1);
}

void VanCarStatusPacketSender::Disable(uint8_t channelId)
{
    vanMessageSender->disable_channel(channelId);
}
