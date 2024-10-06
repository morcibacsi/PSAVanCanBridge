#include "VanMessageSenderUlp.h"

VanMessageSenderUlp::VanMessageSenderUlp(uint8_t vanRxPin, uint8_t vanTxPin)
{
    ulpVanTx = new UlpVanTx((gpio_num_t)vanRxPin, (gpio_num_t)vanTxPin, ULP_VAN_125KBPS);
}

bool VanMessageSenderUlp::set_channel_for_transmit_message(uint8_t channelId, uint16_t identifier, const uint8_t values[], uint8_t messageLength, uint8_t requireAck)
{
    ulpVanTx->SendNormalFrame(identifier, values, messageLength, requireAck);
    return true;
}

bool VanMessageSenderUlp::set_channel_for_receive_message(uint8_t channelId, uint16_t identifier, uint8_t messageLength, uint8_t setAck)
{
    return false;
}

bool VanMessageSenderUlp::set_channel_for_reply_request_message_without_transmission(uint8_t channelId, uint16_t identifier, uint8_t messageLength)
{
    return false;
}

bool VanMessageSenderUlp::set_channel_for_reply_request_message(uint8_t channelId, uint16_t identifier, uint8_t messageLength, uint8_t requireAck)
{
    ulpVanTx->SendReplyRequestFrame(identifier);
    return true;
}

bool VanMessageSenderUlp::set_channel_for_immediate_reply_message(uint8_t channelId, uint16_t identifier, const uint8_t values[], uint8_t messageLength)
{
    return false;
}

bool VanMessageSenderUlp::set_channel_for_deferred_reply_message(uint8_t channelId, uint16_t identifier, const uint8_t values[], uint8_t messageLength, uint8_t setAck)
{
    return false;
}

bool VanMessageSenderUlp::set_channel_for_reply_request_detection_message(uint8_t channelId, uint16_t identifier, uint8_t messageLength)
{
    return false;
}

MessageLengthAndStatusRegister VanMessageSenderUlp::message_available(uint8_t channelId)
{
    MessageLengthAndStatusRegister result;
    return result;
}

void VanMessageSenderUlp::read_message(uint8_t channelId, uint8_t* length, uint8_t buffer[])
{

}

uint8_t VanMessageSenderUlp::get_last_channel()
{
    return 0;
}

void VanMessageSenderUlp::begin()
{
    ulpVanTx->Start();
}

bool VanMessageSenderUlp::reactivate_channel(uint8_t channelId)
{
    return false;
}

void VanMessageSenderUlp::reset_channels()
{

}

void VanMessageSenderUlp::set_value_in_channel(uint8_t channelId, uint8_t index0, uint8_t value)
{

}

void VanMessageSenderUlp::disable_channel(uint8_t channelId)
{

}
