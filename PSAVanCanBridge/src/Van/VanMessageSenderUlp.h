#pragma once

#ifndef _VanMessageSenderUlp_h
    #define _VanMessageSenderUlp_h

#if defined(ARDUINO) && ARDUINO >= 100
    #include <Arduino.h>
#else
    #include "WProgram.h"
#endif

#include <UlpVanTx.h>
#include "IVanMessageSender.h"

/// <summary>
/// VAN message sender using the ULP VAN bus TX library
/// </summary>
class VanMessageSenderUlp : public IVanMessageSender {

UlpVanTx* ulpVanTx;

public:
    /// <summary> Constructor for the ULP VAN bus TX library </summary>
    /// <param name="vanRxPin"> Receive pin number </param>
    /// <param name="vanTxPin"> Transmit pin number </param>
    VanMessageSenderUlp(uint8_t vanRxPin, uint8_t vanTxPin);

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
