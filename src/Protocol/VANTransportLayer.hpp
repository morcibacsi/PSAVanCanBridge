#pragma once

#include <string>
#include <cstdint>

#include "ITransportLayer.hpp"
#include "BusMessage.hpp"
#include "../lib/esp32_ulp_lpc_core_van_tx/LpCoreVanTx.hpp"
#include "../lib/esp32_rmt_van_rx/esp32_arduino_rmt_van_rx.h"
#include "../lib/IVanMessageSender.h"
#include "../Helpers/VanCrcCalculator.hpp"

class VANTransportLayer : public ITransportLayer {
private:
    ESP32_RMT_VAN_RX* _vanRx = nullptr;
    VanCrcCalculator* _crcCalculator = nullptr;
    IVanMessageSender* _vanMessageSender = nullptr;

    uint8_t _network = 0;
    void (*_loggerFunction)(const uint8_t network, const uint8_t direction, const BusMessage& message) = nullptr;

    static constexpr size_t TX_QUEUE_LENGTH = 15;
    static constexpr size_t TX_QUEUE_ITEM_SIZE = sizeof(BusMessage);
    static constexpr uint8_t MAX_RETRY_COUNT = 5;

    QueueHandle_t _txQueue;
    TaskHandle_t _txTaskHandle;

    void TxTask();
public:
    std::string Name() override { return "VAN"; };

    VANTransportLayer(IVanMessageSender* vanMessageSender, uint8_t rxPin, uint8_t dataRxLedIndicatorPin);

    uint8_t SendMessage(const BusMessage& message, bool highPriority = false) override;

    bool ReceiveMessage(BusMessage& message) override;

    bool IsBusAvailable() override;

    void SetLoggerFunction(const uint8_t network, void (*loggerFunction)(const uint8_t network, const uint8_t direction, const BusMessage& message)) override
    {
        _network = network;
        _loggerFunction = loggerFunction;
    }
};