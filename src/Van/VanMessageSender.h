#pragma once

#ifndef _VanMessageSender_h
    #define _VanMessageSender_h

#if defined(ARDUINO) && ARDUINO >= 100
    #include <Arduino.h>
    #include <SPI.h>
#else
    #include "WProgram.h"
#endif

#include <tss463_van.h>
#include "AbstractVanMessageSender.h"

enum VAN_NETWORK {
    VAN_BODY,
    VAN_COMFORT,
};

/// <summary> 
/// This is just an abstraction layer around the VAN library in case you need it
/// It is perfectly fine to use the library directly
/// </summary>
class VanMessageSender : public AbstractVanMessageSender {
    TSS463_VAN* VAN;

public:
    /// <summary> Constructor for the VAN bus library </summary>
    /// <param name="vanPin"> CS (chip select) also known as SS (slave select) pin to use </param>
    /// <param name="spi"> An initialized SPI class </param>
    /// <param name="vanNetwork"> The type of the network we want to connect to </param>
    VanMessageSender(uint8_t vanPin, SPIClass* spi, VAN_NETWORK vanNetwork)
    {
        VAN_SPEED vanSpeed;
        switch (vanNetwork)
        {
            case VAN_BODY:
                vanSpeed = VAN_62K5BPS;
            case VAN_COMFORT:
                vanSpeed = VAN_125KBPS;
        }

        VAN = new TSS463_VAN(vanPin, spi, vanSpeed);
    }

    bool set_channel_for_transmit_message(uint8_t channelId, uint16_t identifier, const uint8_t values[], uint8_t messageLength, uint8_t requireAck) override
    {
        return VAN->set_channel_for_transmit_message(channelId, identifier, values, messageLength, requireAck);
    }

    bool set_channel_for_receive_message(uint8_t channelId, uint16_t identifier, uint8_t messageLength, uint8_t setAck) override
    {
        return VAN->set_channel_for_receive_message(channelId, identifier, messageLength, setAck);
    }

    bool set_channel_for_reply_request_message_without_transmission(uint8_t channelId, uint16_t identifier, uint8_t messageLength) override
    {
        return VAN->set_channel_for_reply_request_message_without_transmission(channelId, identifier, messageLength);
    }

    bool set_channel_for_reply_request_message(uint8_t channelId, uint16_t identifier, uint8_t messageLength, uint8_t requireAck) override
    {
        return VAN->set_channel_for_reply_request_message(channelId, identifier, messageLength, requireAck);
    }

    bool set_channel_for_immediate_reply_message(uint8_t channelId, uint16_t identifier, const uint8_t values[], uint8_t messageLength) override
    {
        return VAN->set_channel_for_immediate_reply_message(channelId, identifier, values, messageLength);
    }

    bool set_channel_for_deferred_reply_message(uint8_t channelId, uint16_t identifier, const uint8_t values[], uint8_t messageLength, uint8_t setAck) override
    {
        return VAN->set_channel_for_deferred_reply_message(channelId, identifier, values, messageLength, setAck);
    }

    bool set_channel_for_reply_request_detection_message(uint8_t channelId, uint16_t identifier, uint8_t messageLength) override
    {
        return VAN->set_channel_for_reply_request_detection_message(channelId, identifier, messageLength);
    }

    MessageLengthAndStatusRegister message_available(uint8_t channelId) override
    {
        return VAN->message_available(channelId);
    }

    void read_message(uint8_t channelId, uint8_t* length, uint8_t buffer[]) override
    {
        VAN->read_message(channelId, length, buffer);
    }

    uint8_t get_last_channel() override
    {
        return VAN->get_last_channel();
    }

    void begin() override
    {
        VAN->begin();
    }

    bool reactivate_channel(uint8_t channelId) override
    {
        return VAN->reactivate_channel(channelId);
    }

    void reset_channels() override
    {
        VAN->reset_channels();
    }

    void set_value_in_channel(uint8_t channelId, uint8_t index0, uint8_t value) override
    {
        VAN->set_value_in_channel(channelId, index0, value);
    }
};

#endif
