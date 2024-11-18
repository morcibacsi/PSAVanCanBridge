#pragma once

#ifndef _CanMessageSenderEsp32Idf_h
    #define _CanMessageSenderEsp32Idf_h

#include "ICanMessageSender.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "driver/twai.h"
#include "../SerialPort/AbstractSerial.h"

class CanMessageSenderEsp32Idf : public ICanMessageSender
{
private:
    const int rx_queue_size = 10;

    uint16_t _prevCanId;
    unsigned long _prevCanIdTime;
    esp_err_t _alertInit;
    twai_handle_t _twai;

    SemaphoreHandle_t canSemaphore;
    SemaphoreHandle_t serialSemaphore;

    AbsSer *_serialPort;

    void PrintToSerial(uint16_t canId, uint8_t ext, uint8_t sizeOfByteArray, uint8_t *byteArray);

public:
    static const uint8_t CAN_CONTROLLER_0 = 0;
    static const uint8_t CAN_CONTROLLER_1 = 1;

    CanMessageSenderEsp32Idf(uint8_t rxPin, uint8_t txPin, AbsSer *serialPort, uint8_t handle);

    void Init() override;

    uint8_t SendMessage(uint16_t canId, uint8_t ext, uint8_t sizeOfByteArray, uint8_t *byteArray) override;

    virtual bool ReadMessage(uint16_t *canId, uint8_t *len, uint8_t *buf) override;
    virtual void Reset() override;
};

#endif
