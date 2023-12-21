#pragma once

#ifndef _VanMessageSenderTSS46x_h
    #define _VanMessageSenderTSS46x_h

#if defined(ARDUINO) && ARDUINO >= 100
    #include <Arduino.h>
    #include <SPI.h>
#else
    #include "WProgram.h"
#endif

#include <tss46x_van.h>
#include <tss463.h>
#include "IVanMessageSender.h"

enum VAN_NETWORK {
    VAN_BODY,
    VAN_COMFORT,
};

/// <summary>
/// This is just an abstraction layer around the VAN library in case you need it
/// It is perfectly fine to use the library directly
/// </summary>
class VanMessageSenderTSS46x : public IVanMessageSender {
    ITss46x* vanSender;
    TSS46X_VAN* VAN;

public:
    /// <summary> Constructor for the VAN bus library </summary>
    /// <param name="vanPin"> CS (chip select) also known as SS (slave select) pin to use </param>
    /// <param name="spi"> An initialized SPI class </param>
    /// <param name="vanNetwork"> The type of the network we want to connect to </param>
    VanMessageSenderTSS46x(uint8_t vanPin, SPIClass* spi, VAN_NETWORK vanNetwork);

    bool set_channel_for_transmit_message(uint8_t channelId, uint16_t identifier, const uint8_t values[], uint8_t messageLength, uint8_t requireAck) override;

    bool set_channel_for_receive_message(uint8_t channelId, uint16_t identifier, uint8_t messageLength, uint8_t setAck) override;

    bool set_channel_for_reply_request_message_without_transmission(uint8_t channelId, uint16_t identifier, uint8_t messageLength) override;

    bool set_channel_for_reply_request_message(uint8_t channelId, uint16_t identifier, uint8_t messageLength, uint8_t requireAck) override;

    bool set_channel_for_immediate_reply_message(uint8_t channelId, uint16_t identifier, const uint8_t values[], uint8_t messageLength) override;

    bool set_channel_for_deferred_reply_message(uint8_t channelId, uint16_t identifier, const uint8_t values[], uint8_t messageLength, uint8_t setAck) override;

    bool set_channel_for_reply_request_detection_message(uint8_t channelId, uint16_t identifier, uint8_t messageLength) override;

    MessageLengthAndStatusRegister message_available(uint8_t channelId) override;

    void read_message(uint8_t channelId, uint8_t* length, uint8_t buffer[]) override;

    uint8_t get_last_channel() override;

    void begin() override;

    bool reactivate_channel(uint8_t channelId) override;

    void reset_channels() override;

    void set_value_in_channel(uint8_t channelId, uint8_t index0, uint8_t value) override;

    void disable_channel(uint8_t channelId) override;
};

#endif
