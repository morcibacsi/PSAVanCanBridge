#pragma once

#include "../BusMessage.hpp"
#include "../IProtocolHandler.hpp"
#include "../ITransportLayer.hpp"
#include "../IMessageHandler.hpp"
#include "../FeedbackSignal.hpp"
#include "../ImmediateSignal.hpp"
#include "../../Helpers/CarState.hpp"
#include "../../Helpers/ConfigFile.hpp"
#include "../../Helpers/SupportedMessageHelperTemplate.hpp"

#include "../AEE2004/Handlers/DIAG/TpMessageHandler_760.hpp"

class DiagnosticsContainer : public IProtocolHandler
{
    private:

    CarState* _carState;  // Car state.
    ITransportLayer* _transportLayer;  // Transport layer (CAN, LIN, etc.)
    ConfigFile* _configFile;

    TpMessageHandler_760* _tpHandler760; // Diagnostic TP handler for 0x760

    ImmediateSignalCallback _immediateSignalCallback;
    FeedbackSignalCallback _feedbackSignalCallback;

    static DiagnosticsContainer* _instance;
    static void FeedbackSignalTrampoline(FeedbackSignal signal)
    {
        if (_instance) _instance->HandleFeedbackSignal(signal);
    }

    void SendImmediateMessage(uint32_t id);

    public:

    DiagnosticsContainer(
        CarState* carState,
        ITransportLayer* transport,
        ConfigFile* configFile
    );

    void RegisterMessageHandlers(ImmediateSignalCallback immediateSignalCallback) override;

    bool ReceiveMessage(BusMessage& message) override;

    void ParseMessage(const BusMessage& message) override;

    void GenerateMessages(MessageDirection direction) override;

    void HandleFeedbackSignal(FeedbackSignal signal);

    bool CanAcceptMessage(const BusMessage& message) override;

    void HandleForwardedMessage(const BusMessage& message) override;

    void UpdateMessages(uint64_t currentTime) override;

    void ProcessImmediateSignal(ImmediateSignal signal) override;

    bool CanParseMessage(const BusMessage& message) override;
};
