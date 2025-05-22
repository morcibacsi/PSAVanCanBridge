#pragma once

#include <cstdint>

#include "BusMessage.hpp"
#include "../Helpers/CarState.hpp"

template<typename Derived>
class IMessageHandler {
public:
    // Generates a specific message based on the car state
    inline BusMessage CallGenerate(CarState* carState) {
        return static_cast<Derived*>(this)->Generate(carState);
    }

    inline void CallParse(CarState* carState, const BusMessage& msg) {
        static_cast<Derived*>(this)->Parse(carState, msg);
    }
};