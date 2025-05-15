#pragma once

#include <vector>
#include <memory>
#include <functional>
#include "BusMessage.hpp"
#include "ITransportLayer.hpp"
#include "MessageScheduler.hpp"
#include "FeedbackSignal.hpp"
#include "ImmediateSignal.hpp"

class IProtocolHandler {

public:
    typedef enum {
        CAN_ACCEPT_UNCHANGED = 0,
        CAN_ACCEPT_CHANGED = 1,
        REJECT = 2,
    } CAN_ACCEPT_MESSAGE;

    typedef enum {
        Source = 0,
        Destination = 1,
    } MessageDirection;

    virtual ~IProtocolHandler() = default;

    virtual void RegisterMessageHandlers(ImmediateSignalCallback immediateSignalCallback) = 0;
    virtual bool ReceiveMessage(BusMessage& message) = 0;

    // Parses an incoming message and updates the CarState.
    virtual void ParseMessage(const BusMessage& message) = 0;

    // Generates outgoing messages based on the CarState.
    virtual void GenerateMessages(MessageDirection direction) = 0;

    // Handles feedback signals that require immediate action.
    virtual void HandleFeedbackSignal(FeedbackSignal signal) = 0;

    virtual bool CanAcceptMessage(const BusMessage& message) { return false; };

    virtual void HandleForwardedMessage(const BusMessage& message) { };

    virtual void UpdateMessages(uint64_t currentTime) = 0;

    virtual void ProcessImmediateSignal(ImmediateSignal signal) = 0;

    virtual bool CanParseMessage(const BusMessage& message) { return false; };
};
