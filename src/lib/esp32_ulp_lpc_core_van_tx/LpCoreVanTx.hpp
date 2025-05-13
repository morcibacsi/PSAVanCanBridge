#pragma once

#ifndef _LpCoreVanTx_hpp
    #define _LpCoreVanTx_hpp

#include <stdint.h>
#include "esp_sleep.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"

class LpCoreVanTx
{
    public:

    typedef enum {
        LP_VAN_62K5BPS  = 0,
        LP_VAN_125KBPS  = 1,
    } LP_VAN_NETWORK_SPEED;

private:
    void PrintToSerial(uint16_t canId, uint8_t ext, uint8_t sizeOfByteArray, uint8_t *byteArray);

    private:
        gpio_num_t _rxPin;
        gpio_num_t _txPin;
        LP_VAN_NETWORK_SPEED _networkSpeed;

        uint16_t Crc15(const uint8_t data[], const uint8_t length);
        void InternalSendFrame(const uint8_t retryCount, const uint8_t data[], const uint8_t length);
    public:
        LpCoreVanTx(gpio_num_t rxPin, gpio_num_t txPin, LP_VAN_NETWORK_SPEED networkSpeed);
        ~LpCoreVanTx();
        void Start();
        void SendNormalFrame(const uint16_t identifier, const uint8_t data[], const uint8_t length, const bool requireAck);
        void SendReplyRequestFrame(const uint16_t identifier);
        bool IsTxPossible();
};

#endif
