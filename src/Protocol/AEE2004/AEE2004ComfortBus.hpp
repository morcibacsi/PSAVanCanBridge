#pragma once

#include <vector>
#include <memory>
#include "../BusMessage.hpp"
#include "../IProtocolHandler.hpp"
#include "../ITransportLayer.hpp"
#include "../IMessageHandler.hpp"
#include "../MessageScheduler.hpp"
#include "../FeedbackSignal.hpp"
#include "../ImmediateSignal.hpp"
#include "../../Helpers/CarState.hpp"

class AEE2004ComfortBus : public IProtocolHandler
{
    private:
    static const int MAX_CAN_ID = 0x3B6 + 1;

    CarState* _carState;  // Car state.
    ITransportLayer* _transportLayer;  // Transport layer (CAN, LIN, etc.)
    MessageScheduler* _scheduler;  // Message scheduler injected via constructor.
    IMessageHandler* _messageHandlers[MAX_CAN_ID]{};

    ImmediateSignalCallback _immediateSignalCallback;

    void SendImmediateMessage(uint32_t id);

    public:
    AEE2004ComfortBus(
        CarState* carState,
        ITransportLayer* transport,
        MessageScheduler* scheduler
    );

    void RegisterMessageHandlers(ImmediateSignalCallback immediateSignalCallback) override;

    bool ReceiveMessage(BusMessage& message) override;

    void ParseMessage(const BusMessage& message) override;

    void GenerateMessages(MessageDirection direction) override;

    void HandleFeedbackSignal(FeedbackSignal signal);

    void UpdateMessages(uint64_t currentTime) override;

    void ProcessImmediateSignal(ImmediateSignal signal) override;

    bool CanParseMessage(const BusMessage& message) override;
};