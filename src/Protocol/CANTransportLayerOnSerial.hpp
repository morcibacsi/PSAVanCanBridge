#pragma once

#include <string>
#include "ITransportLayer.hpp"
#include "BusMessage.hpp"
#include "../lib/esp32_ulp_lpc_core_van_tx/LpCoreVanTx.hpp"
#include "../lib/esp32_rmt_van_rx/esp32_arduino_rmt_van_rx.h"

class CANTransportLayerOnSerial : public ITransportLayer {
private:
    #define BUF_SIZE (1024)
    #define ECHO_TASK_STACK_SIZE (4096)
    uint8_t data[BUF_SIZE];
    uint16_t FastChecksum(const uint8_t *data, uint8_t length);
public:
    std::string Name() override { return "CAN"; };

    CANTransportLayerOnSerial();

    void SendMessage(const BusMessage& message, bool highPriority = false) override;

    bool ReceiveMessage(BusMessage& message) override;

    bool IsBusAvailable() override;
};