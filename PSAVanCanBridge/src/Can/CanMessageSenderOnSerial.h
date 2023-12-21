#pragma once

#ifndef _CanMessageSenderOnSerial_h
    #define _CanMessageSenderOnSerial_h

#include "ICanMessageSender.h"
#include "../SerialPort/AbstractSerial.h"

class CanMessageSenderOnSerial : public ICanMessageSender
{
private:
    AbsSer *_serialPort;

    void PrintToSerial(uint16_t canId, uint8_t ext, uint8_t sizeOfByteArray, uint8_t *byteArray);

public:
    CanMessageSenderOnSerial(AbsSer *serialPort);

    void Init() override;

    uint8_t SendMessage(uint16_t canId, uint8_t ext, uint8_t sizeOfByteArray, uint8_t *byteArray) override;

    void ReadMessage(uint16_t *canId, uint8_t *len, uint8_t *buf) override;
};

#endif
