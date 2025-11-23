#pragma once
#include <cstdint>

class IVanMessageSender
{
    public:
        virtual void Start() = 0;
        virtual void SendNormalFrame(const uint16_t identifier, const uint8_t data[], const uint8_t length, const bool requireAck) = 0;
        virtual void SendReplyRequestFrame(const uint16_t identifier) = 0;
        virtual bool IsTxPossible() = 0;
};