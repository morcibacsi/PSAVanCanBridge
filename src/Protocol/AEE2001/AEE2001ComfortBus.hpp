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
#include "Handlers/BSI/CanDisplayPopupHandler3.h"

class AEE2001ComfortBus : public IProtocolHandler {
private:
    static const int MAX_CAN_ID = 0xAE8 + 1;

    CarState* _carState;  // Car state.
    ITransportLayer* _transportLayer;  // Transport layer (CAN, LIN, etc.)
    MessageScheduler* _schedulerForSourceNetwork;  // Message scheduler injected via constructor.
    CanDisplayPopupHandler3* _canPopupHandler;

    IMessageHandler* _messageHandlers[MAX_CAN_ID]{};
    IMessageHandler* _messageHandlersForSource[MAX_CAN_ID]{};

    ImmediateSignalCallback _immediateSignalCallback;
    FeedbackSignalCallback _feedbackSignalCallback;

    static AEE2001ComfortBus* _instance;
    static void FeedbackSignalTrampoline(FeedbackSignal signal)
    {
        if (_instance) _instance->HandleFeedbackSignal(signal);
    }

    void GenerateMessagesForSource();
    public:
    AEE2001ComfortBus(
        CarState* carState,
        ITransportLayer* transport,
        MessageScheduler* scheduler
    );
    void RegisterMessageHandlers(ImmediateSignalCallback immediateSignalCallback) override;

    bool ReceiveMessage(BusMessage& message) override;

    void ParseMessage(const BusMessage& message) override;

    void GenerateMessages(MessageDirection direction) override;

    void HandleFeedbackSignal(FeedbackSignal signal) override;

    void UpdateMessages(uint64_t currentTime);

    void ProcessImmediateSignal(ImmediateSignal signal) override;

    bool CanParseMessage(const BusMessage& message) override;
};