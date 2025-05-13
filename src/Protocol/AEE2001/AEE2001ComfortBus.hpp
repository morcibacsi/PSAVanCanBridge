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
    std::shared_ptr<CarState> _carState;  // Car state.
    std::shared_ptr<ITransportLayer> _transportLayer;  // Transport layer (CAN, LIN, etc.)
    std::shared_ptr<MessageScheduler> _schedulerForSourceNetwork;  // Message scheduler injected via constructor.
    std::shared_ptr<CanDisplayPopupHandler3> _canPopupHandler;
    std::unordered_map<uint32_t, std::shared_ptr<IMessageHandler>> _messageHandlers;
    std::unordered_map<uint32_t, std::shared_ptr<IMessageHandler>> _messageHandlersForSource;

    std::function<void(ImmediateSignal)> _immediateSignalCallback;

    void GenerateMessagesForSource();
    public:
    AEE2001ComfortBus(
        std::shared_ptr<CarState> carState,
        std::shared_ptr<ITransportLayer> transport,
        std::shared_ptr<MessageScheduler> scheduler
    );
    void RegisterMessageHandlers(std::function<void(ImmediateSignal)> immediateSignalCallback) override;

    bool ReceiveMessage(BusMessage& message) override;

    void ParseMessage(const BusMessage& message) override;

    void GenerateMessages(MessageDirection direction) override;

    void HandleFeedbackSignal(FeedbackSignal signal) override;

    void UpdateMessages(uint64_t currentTime);

    void ProcessImmediateSignal(ImmediateSignal signal) override;

    bool CanParseMessage(const BusMessage& message) override;
};