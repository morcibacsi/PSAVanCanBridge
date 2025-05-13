#pragma once

#include <inttypes.h>
#include <memory>
#include "BusMessage.hpp"
#include "../Helpers/CarState.hpp"

class IMessageHandler {
public:
    virtual ~IMessageHandler() = default;

    // Generates a specific message based on the car state
    virtual BusMessage Generate(std::shared_ptr<CarState> state) = 0;
    virtual void Parse(std::shared_ptr<CarState> state, const BusMessage& message) { };
};