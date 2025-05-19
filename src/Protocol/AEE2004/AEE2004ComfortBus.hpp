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
#include "../../Helpers/SupportedMessageHelperTemplate.hpp"

#include "Handlers/AAS/MessageHandler_0E1.hpp"
#include "Handlers/BSI/MessageHandler_0B6.h"
#include "Handlers/BSI/MessageHandler_0E6.h"
#include "Handlers/BSI/MessageHandler_0E8.h"
#include "Handlers/BSI/MessageHandler_0F6.h"
#include "Handlers/BSI/MessageHandler_1A1.h"
#include "Handlers/BSI/MessageHandler_1A8.h"
#include "Handlers/BSI/MessageHandler_2A1.h"
#include "Handlers/BSI/MessageHandler_2B6.h"
#include "Handlers/BSI/MessageHandler_2E1.h"
#include "Handlers/BSI/MessageHandler_3A7.h"
#include "Handlers/BSI/MessageHandler_3B6.h"
#include "Handlers/BSI/MessageHandler_10B.h"
#include "Handlers/BSI/MessageHandler_21F.h"
#include "Handlers/BSI/MessageHandler_036.h"
#include "Handlers/BSI/MessageHandler_120.h"
#include "Handlers/BSI/MessageHandler_127.h"
#include "Handlers/BSI/MessageHandler_128.h"
#include "Handlers/BSI/MessageHandler_161.h"
#include "Handlers/BSI/MessageHandler_168.h"
#include "Handlers/CMB/MessageHandler_217.h"
#include "Handlers/BSI/MessageHandler_220.h"
#include "Handlers/BSI/MessageHandler_221.h"
#include "Handlers/BSI/MessageHandler_227.h"
#include "Handlers/BSI/MessageHandler_261.h"
#include "Handlers/BSI/MessageHandler_336.h"
#include "Handlers/BSI/MessageHandler_361.h"
#include "Handlers/CLV/MessageHandler_1E3.h"
#include "Handlers/EMF/MessageHandler_167.h"
#include "Handlers/VTH/MessageHandler_297.h"

class AEE2004ComfortBus : public IProtocolHandler
{
    private:

    CarState* _carState;  // Car state.
    ITransportLayer* _transportLayer;  // Transport layer (CAN, LIN, etc.)
    MessageScheduler* _scheduler;  // Message scheduler injected via constructor.

    ImmediateSignalCallback _immediateSignalCallback;

    void SendImmediateMessage(uint32_t id);

    public:
    std::tuple<
        MessageHandler_0E1,
        MessageHandler_0B6,
        MessageHandler_0E6,
        MessageHandler_0E8,
        MessageHandler_0F6,
        MessageHandler_1A1,
        MessageHandler_1A8,
        MessageHandler_2A1,
        MessageHandler_2B6,
        MessageHandler_2E1,
        MessageHandler_3A7,
        MessageHandler_3B6,
        MessageHandler_10B,
        MessageHandler_21F,
        MessageHandler_036,
        MessageHandler_120,
        MessageHandler_127,
        MessageHandler_128,
        MessageHandler_161,
        MessageHandler_168,
        MessageHandler_217,
        MessageHandler_220,
        MessageHandler_221,
        MessageHandler_227,
        MessageHandler_261,
        MessageHandler_336,
        MessageHandler_361,
        MessageHandler_1E3,
        MessageHandler_167,
        MessageHandler_297
    > handlers;

    constexpr static auto SupportedMessageIds = ExtractMessageIds<decltype(handlers)>();

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