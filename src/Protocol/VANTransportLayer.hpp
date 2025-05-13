#pragma once

#include <string>
#include "ITransportLayer.hpp"
#include "BusMessage.hpp"
#include "../lib/esp32_ulp_lpc_core_van_tx/LpCoreVanTx.hpp"
#include "../lib/esp32_rmt_van_rx/esp32_arduino_rmt_van_rx.h"
#include "../Helpers/VanCrcCalculator.h"

class VANTransportLayer : public ITransportLayer {
private:
    LpCoreVanTx* _vanTx = nullptr;
    ESP32_RMT_VAN_RX* _vanRx = nullptr;
    VanCrcCalculator* _crcCalculator = nullptr;

    static constexpr size_t TX_QUEUE_LENGTH = 15;
    static constexpr size_t TX_QUEUE_ITEM_SIZE = sizeof(BusMessage);
    static constexpr uint8_t MAX_RETRY_COUNT = 5;

    QueueHandle_t _txQueue;
    TaskHandle_t _txTaskHandle;

    void TxTask();
public:
    std::string Name() override { return "VAN"; };

    VANTransportLayer(uint8_t rxPin, uint8_t txPin, uint8_t dataRxLedIndicatorPin);

    void SendMessage(const BusMessage& message, bool highPriority = false) override;

    bool ReceiveMessage(BusMessage& message) override;

    bool IsBusAvailable() override;
};