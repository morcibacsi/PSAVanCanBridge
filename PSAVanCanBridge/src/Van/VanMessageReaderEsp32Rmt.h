#pragma once

#ifndef _VanMessageReaderEsp32Rmt_h
    #define _VanMessageReaderEsp32Rmt_h

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

#include "IVanMessageReader.h"
#include <esp32_arduino_rmt_van_rx.h>

class VanMessageReaderEsp32Rmt : public IVanMessageReader {
    const uint8_t VAN_DATA_RX_RMT_CHANNEL = 0;

    ESP32_RMT_VAN_RX *_van_rx;
public:
    VanMessageReaderEsp32Rmt(uint8_t rxPin, uint8_t ledPin, IVAN_LINE_LEVEL vanLineLevel, IVAN_NETWORK_TYPE vanNetworkType)
    {
        _van_rx = new ESP32_RMT_VAN_RX(VAN_DATA_RX_RMT_CHANNEL, rxPin, ledPin, static_cast<VAN_LINE_LEVEL>(vanLineLevel), static_cast<VAN_NETWORK_TYPE>(vanNetworkType));
    }

    void Receive(uint8_t* messageLength, uint8_t message[]) override
    {
        _van_rx->ReceiveData(messageLength, message);
    }

    void Init() override
    {
        _van_rx->Start();
    }

    void Stop() override
    {
        _van_rx->Stop();
    }

    bool IsCrcOk(uint8_t vanMessage[], uint8_t vanMessageLength) override
    {
        return _van_rx->IsCrcOk(vanMessage, vanMessageLength);
    }
};

#endif
