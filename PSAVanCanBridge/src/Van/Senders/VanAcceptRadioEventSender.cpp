#include "VanAcceptRadioEventSender.h"

VanAcceptRadioEventSender::VanAcceptRadioEventSender(IVanMessageSender * object)
{
    vanMessageSender = object;
}

bool VanAcceptRadioEventSender::Accept(uint8_t channelId)
{
    vanMessageSender->set_channel_for_receive_message(channelId, 0x8C4, 3, 1);
}

void VanAcceptRadioEventSender::Disable(uint8_t channelId)
{
    vanMessageSender->disable_channel(channelId);
}
