#pragma once

#include <inttypes.h>
#include <memory>
#include "BusMessage.hpp"
#include "../Helpers/CarState.hpp"

template<typename Derived>
class IMessageHandler {
public:
    // Generates a specific message based on the car state
    inline BusMessage CallGenerate(CarState* state) {
        return static_cast<Derived*>(this)->Generate(state);
    }

    inline void CallParse(CarState* state, const BusMessage& msg) {
        static_cast<Derived*>(this)->Parse(state, msg);
    }
};