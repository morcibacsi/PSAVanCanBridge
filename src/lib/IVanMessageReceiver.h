#pragma once

#include <stdint.h>

static constexpr uint8_t VAN_RX_MAX_MESSAGE_BYTES = 33;

class IVanMessageReceiver
{
    public:
        virtual ~IVanMessageReceiver() = default;

        // Blocks until one receiver item is available. A zero length means the
        // item did not contain a complete VAN frame. The output contains the
        // logical SOF, identifier/COM, DATA and packed 15-bit FCS bytes.
        virtual void ReceiveData(uint8_t* messageLength, uint8_t message[]) = 0;
};
