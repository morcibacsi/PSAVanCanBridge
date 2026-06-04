#pragma once

#include <string>
#include <algorithm>
#include <cstdint>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "ITransportLayer.hpp"
#include "BusMessage.hpp"
#include "../lib/CanMessageSender/ICanMessageSender.h"
#include "../Helpers/VanCrcCalculator.hpp"

class CANTransportLayer : public ITransportLayer {
private:
    ICanMessageSender* _canMessageSender;
    uint16_t FastChecksum(const uint8_t *data, uint8_t length);
    VanCrcCalculator* _crcCalculator = nullptr;

    uint8_t _network = 0;
    void (*_loggerFunction)(const uint8_t network, const uint8_t direction, const BusMessage& message) = nullptr;

    SemaphoreHandle_t serialSemaphore;

    void PrintToSerial(uint16_t canId, uint8_t ext, uint8_t sizeOfByteArray, const uint8_t byteArray[]);
public:
    std::string Name() override { return "CAN"; };

    CANTransportLayer(ICanMessageSender* canMessageSender);

    uint8_t SendMessage(const BusMessage& message, bool highPriority = false) override;

    bool ReceiveMessage(BusMessage& message) override;

    bool IsBusAvailable() override
    {
        return true;
        //return CheckCanBusAvailability();
    }

    void SetLoggerFunction(const uint8_t network, void (*loggerFunction)(const uint8_t network,const uint8_t direction, const BusMessage& message)) override
    {
        _network = network;
        _loggerFunction = loggerFunction;
    }
};