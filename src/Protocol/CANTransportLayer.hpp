#pragma once

#include <string>
#include <algorithm>
#include "ITransportLayer.hpp"
#include "BusMessage.hpp"
#include <lib/CanMessageSender/ICanMessageSender.h>
#include "../Helpers/VanCrcCalculator.h"

class CANTransportLayer : public ITransportLayer {
private:
    ICanMessageSender* _canMessageSender;
    uint16_t FastChecksum(const uint8_t *data, uint8_t length);
    VanCrcCalculator* _crcCalculator = nullptr;

public:
    std::string Name() override { return "CAN"; };

    CANTransportLayer(uint8_t rxPin, uint8_t txPin, uint8_t canController);

    void SendMessage(const BusMessage& message, bool highPriority = false) override;

    bool ReceiveMessage(BusMessage& message) override;

    bool IsBusAvailable() override
    {
        return true;
        //return CheckCanBusAvailability();
    }
};