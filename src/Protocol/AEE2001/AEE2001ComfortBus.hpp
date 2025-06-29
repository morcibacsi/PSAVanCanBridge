#pragma once

#include "../BusMessage.hpp"
#include "../IProtocolHandler.hpp"
#include "../ITransportLayer.hpp"
#include "../IMessageHandler.hpp"
#include "../MessageScheduler.hpp"
#include "../FeedbackSignal.hpp"
#include "../ImmediateSignal.hpp"
#include "../../Helpers/CarState.hpp"
#include "../../Helpers/SupportedMessageHelperTemplate.hpp"
#include "Handlers/BSI/CanDisplayPopupHandler3.hpp"

#include "Handlers/BSI/MessageHandler_4FC.hpp"
#include "Handlers/BSI/MessageHandler_4DC.hpp"
#include "Handlers/BSI/MessageHandler_8A4.hpp"
#include "Handlers/BSI/MessageHandler_8C4.hpp"
#include "Handlers/BSI/MessageHandler_9C4.hpp"
#include "Handlers/BSI/MessageHandler_524.hpp"
#include "Handlers/BSI/MessageHandler_564.hpp"
#include "Handlers/BSI/MessageHandler_744.hpp"
#include "Handlers/BSI/MessageHandler_824.hpp"
#include "Handlers/BSI/MessageHandler_A68.hpp"
#include "Handlers/BSI/MessageHandler_AE8.hpp"
#include "Handlers/BSI/MessageHandler_ADC.hpp"
#include "Handlers/BSI/MessageHandler_A5C.hpp"

#include "Handlers/CLIM/MessageHandler_464.hpp"

#include "Handlers/EMF/MessageHandler_5E4.hpp"

class AEE2001ComfortBus : public IProtocolHandler
{
    private:

    CarState* _carState;  // Car state.
    ITransportLayer* _transportLayer;  // Transport layer (CAN, LIN, etc.)
    MessageScheduler* _schedulerForSourceNetwork;  // Message scheduler injected via constructor.
    CanDisplayPopupHandler3* _canPopupHandler;

    ImmediateSignalCallback _immediateSignalCallback;
    FeedbackSignalCallback _feedbackSignalCallback;

    static AEE2001ComfortBus* _instance;
    static void FeedbackSignalTrampoline(FeedbackSignal signal)
    {
        if (_instance) _instance->HandleFeedbackSignal(signal);
    }

    void GenerateMessagesForSource();

    public:
    std::tuple<
        MessageHandler_4FC,
        MessageHandler_4DC,
        MessageHandler_8A4,
        MessageHandler_8C4,
        MessageHandler_9C4,
        MessageHandler_524,
        MessageHandler_5E4,
        MessageHandler_564,
        MessageHandler_744,
        MessageHandler_824,
        MessageHandler_A68,
        MessageHandler_AE8,
        MessageHandler_A5C,
        MessageHandler_ADC,
        MessageHandler_464
    > handlers;

    constexpr static auto SupportedMessageIds = ExtractMessageIds<decltype(handlers)>();

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