#pragma once

#ifndef _CanMessageSenderEsp32Idf_h
    #define _CanMessageSenderEsp32Idf_h

#include "AbstractCanMessageSender.h"
#include "../SerialPort/AbstractSerial.h"

class CanMessageSenderEsp32Idf : public AbstractCanMessageSender
{
private:
    const int rx_queue_size = 10;

    uint16_t _prevCanId;
    unsigned long _prevCanIdTime;
    bool _enableThrottling;

    SemaphoreHandle_t canSemaphore;

    AbsSer *_serialPort;

    void PrintToSerial(uint16_t canId, uint8_t ext, uint8_t sizeOfByteArray, uint8_t *byteArray);

public:
    CanMessageSenderEsp32Idf(uint8_t rxPin, uint8_t txPin, bool enableThrottling, AbsSer *serialPort);

    void Init() override;

    uint8_t SendMessage(uint16_t canId, uint8_t ext, uint8_t sizeOfByteArray, uint8_t *byteArray) override;

    virtual void ReadMessage(uint16_t *canId, uint8_t *len, uint8_t *buf) override;
};

#endif
