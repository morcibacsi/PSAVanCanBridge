#pragma once

#include <vector>

#include "../BusMessage.hpp"
#include "../IProtocolHandler.hpp"
#include "../ITransportLayer.hpp"
#include "../IMessageHandler.hpp"
#include "../MessageScheduler.hpp"
#include "../FeedbackSignal.hpp"
#include "../ImmediateSignal.hpp"
#include "../../Helpers/CarState.hpp"
#include "../../Helpers/SupportedMessageHelperTemplate.hpp"

#include "Handlers/AAS/MessageHandler_0E1_2010.hpp"

#include "Handlers/BSI/MessageHandler_0B6_2010.hpp"
#include "Handlers/BSI/MessageHandler_0E6_2010.hpp"
#include "Handlers/BSI/MessageHandler_0F6_2010.hpp"
#include "Handlers/BSI/MessageHandler_1A1_2010.hpp"
#include "Handlers/BSI/MessageHandler_1A8_2010.hpp"
#include "Handlers/BSI/MessageHandler_2B6_2010.hpp"
#include "Handlers/BSI/MessageHandler_2E1_2010.hpp"
#include "Handlers/BSI/MessageHandler_3B6_2010.hpp"
#include "Handlers/BSI/MessageHandler_3E7_2010.hpp"
#include "Handlers/BSI/MessageHandler_10B_2010.hpp"
#include "Handlers/BSI/MessageHandler_21F_2010.hpp"
#include "Handlers/BSI/MessageHandler_036_2010.hpp"
#include "Handlers/BSI/MessageHandler_120_2010.hpp"
#include "Handlers/BSI/MessageHandler_128_2010.hpp"
#include "Handlers/BSI/MessageHandler_161_2010.hpp"
#include "Handlers/BSI/MessageHandler_168_2010.hpp"
#include "Handlers/BSI/MessageHandler_2A1_2010.hpp"
#include "Handlers/BSI/MessageHandler_221_2010.hpp"
#include "Handlers/BSI/MessageHandler_227_2010.hpp"
#include "Handlers/BSI/MessageHandler_228_2010.hpp"
#include "Handlers/BSI/MessageHandler_236_2010.hpp"
#include "Handlers/BSI/MessageHandler_260_2010.hpp"
#include "Handlers/BSI/MessageHandler_261_2010.hpp"
#include "Handlers/BSI/MessageHandler_276_2010.hpp"
#include "Handlers/BSI/MessageHandler_336_2010.hpp"
#include "Handlers/BSI/MessageHandler_361_2010.hpp"

#include "Handlers/CMB/MessageHandler_122_2010.hpp"
#include "Handlers/CMB/MessageHandler_217_2010.hpp"

class AEE2010ComfortBus : public IProtocolHandler
{
    private:

    CarState* _carState;  // Car state.
    ITransportLayer* _transportLayer;  // Transport layer (CAN, LIN, etc.)
    MessageScheduler* _scheduler;  // Message scheduler injected via constructor.
    std::vector<uint32_t> _messagesToSkip;
    std::vector<uint32_t> _messagesToForward;

    ImmediateSignalCallback _immediateSignalCallback;

    void SendImmediateMessage(uint32_t id);

    public:
    std::tuple<
        MessageHandler_0B6_2010,
        MessageHandler_0E1_2010,
        MessageHandler_0E6_2010,
        MessageHandler_0F6_2010,
        MessageHandler_1A1_2010,
        MessageHandler_1A8_2010,
        MessageHandler_2B6_2010,
        MessageHandler_2E1_2010,
        MessageHandler_3B6_2010,
        MessageHandler_3E7_2010,
        MessageHandler_10B_2010,
        MessageHandler_21F_2010,
        MessageHandler_036_2010,
        MessageHandler_120_2010,
        MessageHandler_128_2010,
        MessageHandler_161_2010,
        MessageHandler_168_2010,
        MessageHandler_2A1_2010,
        MessageHandler_221_2010,
        MessageHandler_227_2010,
        MessageHandler_228_2010,
        MessageHandler_236_2010,
        MessageHandler_260_2010,
        MessageHandler_261_2010,
        MessageHandler_276_2010,
        MessageHandler_336_2010,
        MessageHandler_361_2010,
        MessageHandler_122_2010,
        MessageHandler_217_2010
    > handlers;

    constexpr static auto SupportedMessageIds = ExtractMessageIds<decltype(handlers)>();

    AEE2010ComfortBus(
        CarState* carState,
        ITransportLayer* transport,
        MessageScheduler* scheduler
        );

    void RegisterMessageHandlers(ImmediateSignalCallback immediateSignalCallback) override;

    bool ReceiveMessage(BusMessage& message) override;

    void ParseMessage(const BusMessage& message) override;

    void GenerateMessages(MessageDirection direction) override;

    void HandleFeedbackSignal(FeedbackSignal signal) override;

    bool CanAcceptMessage(const BusMessage& message) override;

    void HandleForwardedMessage(const BusMessage& message) override;

    void UpdateMessages(uint64_t currentTime) override;

    void ProcessImmediateSignal(ImmediateSignal signal) override;

    bool CanParseMessage(const BusMessage& message) override;
};