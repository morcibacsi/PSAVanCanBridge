#pragma once
#include <cstdint>

class IVanMessageSender
{
    public:
        virtual void Start() = 0;
        virtual void SendNormalFrame(const uint16_t identifier, const uint8_t data[], const uint8_t length, const bool requireAck) = 0;
        virtual void SendReplyRequestFrame(const uint16_t identifier) = 0;
        virtual bool IsTxPossible() = 0;

        // Optional capabilities for ULP/LP-core based VAN helpers.
        // Implementations that do not support these features can keep defaults.
        virtual void SetAckIdentifiers(const uint16_t identifiers[], const uint8_t count) { (void)identifiers; (void)count; }
        virtual void SetQueryRequesterAckEnabled(const bool enabled) { (void)enabled; }
        virtual void SetRequestedReplyFrame(const uint16_t identifier, const uint8_t data[], const uint8_t length, const bool enabled)
        {
            (void)identifier;
            (void)data;
            (void)length;
            (void)enabled;
        }
};