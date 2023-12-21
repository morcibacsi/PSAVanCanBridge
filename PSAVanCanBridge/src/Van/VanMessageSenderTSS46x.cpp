#include "VanMessageSenderTSS46x.h"

VanMessageSenderTSS46x::VanMessageSenderTSS46x(uint8_t vanPin, SPIClass* spi, VAN_NETWORK vanNetwork)
{
    VAN_SPEED vanSpeed;
    switch (vanNetwork)
    {
        case VAN_BODY:
            vanSpeed = VAN_62K5BPS;
        case VAN_COMFORT:
            vanSpeed = VAN_125KBPS;
    }

    vanSender = new Tss463(vanPin, spi);
    VAN = new TSS46X_VAN(vanSender, vanSpeed);
}

bool VanMessageSenderTSS46x::set_channel_for_transmit_message(uint8_t channelId, uint16_t identifier, const uint8_t values[], uint8_t messageLength, uint8_t requireAck)
{
    return VAN->set_channel_for_transmit_message(channelId, identifier, values, messageLength, requireAck);
}

bool VanMessageSenderTSS46x::set_channel_for_receive_message(uint8_t channelId, uint16_t identifier, uint8_t messageLength, uint8_t setAck)
{
    return VAN->set_channel_for_receive_message(channelId, identifier, messageLength, setAck);
}

bool VanMessageSenderTSS46x::set_channel_for_reply_request_message_without_transmission(uint8_t channelId, uint16_t identifier, uint8_t messageLength)
{
    return VAN->set_channel_for_reply_request_message_without_transmission(channelId, identifier, messageLength);
}

bool VanMessageSenderTSS46x::set_channel_for_reply_request_message(uint8_t channelId, uint16_t identifier, uint8_t messageLength, uint8_t requireAck)
{
    return VAN->set_channel_for_reply_request_message(channelId, identifier, messageLength, requireAck);
}

bool VanMessageSenderTSS46x::set_channel_for_immediate_reply_message(uint8_t channelId, uint16_t identifier, const uint8_t values[], uint8_t messageLength)
{
    return VAN->set_channel_for_immediate_reply_message(channelId, identifier, values, messageLength);
}

bool VanMessageSenderTSS46x::set_channel_for_deferred_reply_message(uint8_t channelId, uint16_t identifier, const uint8_t values[], uint8_t messageLength, uint8_t setAck)
{
    return VAN->set_channel_for_deferred_reply_message(channelId, identifier, values, messageLength, setAck);
}

bool VanMessageSenderTSS46x::set_channel_for_reply_request_detection_message(uint8_t channelId, uint16_t identifier, uint8_t messageLength)
{
    return VAN->set_channel_for_reply_request_detection_message(channelId, identifier, messageLength);
}

MessageLengthAndStatusRegister VanMessageSenderTSS46x::message_available(uint8_t channelId)
{
    return VAN->message_available(channelId);
}

void VanMessageSenderTSS46x::read_message(uint8_t channelId, uint8_t* length, uint8_t buffer[])
{
    VAN->read_message(channelId, length, buffer);
}

uint8_t VanMessageSenderTSS46x::get_last_channel()
{
    return VAN->get_last_channel();
}

void VanMessageSenderTSS46x::begin()
{
    VAN->begin();
}

bool VanMessageSenderTSS46x::reactivate_channel(uint8_t channelId)
{
    return VAN->reactivate_channel(channelId);
}

void VanMessageSenderTSS46x::reset_channels()
{
    VAN->reset_channels();
}

void VanMessageSenderTSS46x::set_value_in_channel(uint8_t channelId, uint8_t index0, uint8_t value)
{
    VAN->set_value_in_channel(channelId, index0, value);
}

void VanMessageSenderTSS46x::disable_channel(uint8_t channelId)
{
    VAN->disable_channel(channelId);
}
