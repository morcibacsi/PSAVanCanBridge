#pragma once

#ifndef _CanMessageSenderMcp2515Idf_h
    #define _CanMessageSenderMcp2515Idf_h

#include "ICanMessageSender.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "../../../components/esp32-mcp2515-main/include/mcp2515.h"

class CanMessageSenderMcp2515Idf : public ICanMessageSender
{
private:
    const int rx_queue_size = 10;

    MCP2515* _mcp2515;
    spi_device_handle_t _spiHandle;

    uint8_t _handle;

    SemaphoreHandle_t canSemaphore;

public:
    CanMessageSenderMcp2515Idf(uint8_t misoPin, uint8_t mosiPin, uint8_t clkPin, uint8_t csPin, spi_host_device_t spiHost);

    void Init() override;

    uint8_t SendMessage(uint16_t canId, uint8_t ext, uint8_t sizeOfByteArray, const uint8_t byteArray[]) override;

    virtual bool ReadMessage(uint16_t *canId, uint8_t *len, uint8_t *buf) override;
    virtual void Reset() override;
};

#endif
