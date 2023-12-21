#pragma once

#ifndef _VanMessageReaderEsp32Rmt_h
    #define _VanMessageReaderEsp32Rmt_h

#include "IVanMessageReader.h"
#include "esp32_arduino_rmt_van_rx.h"

class VanMessageReaderEsp32Rmt : public IVanMessageReader {
    const uint8_t VAN_DATA_RX_RMT_CHANNEL = 0;

    ESP32_RMT_VAN_RX *_van_rx;
public:
    VanMessageReaderEsp32Rmt(uint8_t rxPin, uint8_t ledPin, IVAN_LINE_LEVEL vanLineLevel, IVAN_NETWORK_TYPE vanNetworkType);

    void Receive(uint8_t* messageLength, uint8_t message[]) override;

    void Init() override;

    void Stop() override;

    bool IsCrcOk(uint8_t vanMessage[], uint8_t vanMessageLength) override;
};

#endif
