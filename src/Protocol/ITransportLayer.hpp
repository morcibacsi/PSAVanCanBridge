#pragma once

#include <string>
#include "BusMessage.hpp"

class ITransportLayer {
public:
    virtual std::string Name() = 0;

    virtual ~ITransportLayer() = default;

    // Sends a message over the bus.
    virtual void SendMessage(const BusMessage& message, bool highPriority = false) = 0;

    // Receives a message from the bus.
    virtual bool ReceiveMessage(BusMessage& message) = 0;

    // Indicates if the bus is available for communication.
    virtual bool IsBusAvailable() = 0;
};
