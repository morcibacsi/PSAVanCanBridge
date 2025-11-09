#include "DiagnosticsContainer.hpp"

DiagnosticsContainer* DiagnosticsContainer::_instance = nullptr;

DiagnosticsContainer::DiagnosticsContainer(
        CarState* carState,
        ITransportLayer* transport,
        ConfigFile* configFile
        )
{
    _instance = this;
    _carState = carState;
    _transportLayer = transport;
    _configFile = configFile;
    _feedbackSignalCallback = &FeedbackSignalTrampoline;
    _immediateSignalCallback = nullptr;

    _tpHandler760 = new TpMessageHandler_760(_transportLayer, _carState);
    _tpHandler760->SetFeedbackSignalCallback(_feedbackSignalCallback);
}

void DiagnosticsContainer::RegisterMessageHandlers(ImmediateSignalCallback immediateSignalCallback)
{
    _immediateSignalCallback = immediateSignalCallback;
}

bool IRAM_ATTR DiagnosticsContainer::ReceiveMessage(BusMessage& message)
{
    return false;
}

void IRAM_ATTR DiagnosticsContainer::ParseMessage(const BusMessage& message)
{
    if (_carState == nullptr)
    {
        return;
    }
    //printf("DiagnosticsContainer::ParseMessage: %X\n", (unsigned int)message.id);
    _tpHandler760->Receive(_carState->CurrenTime, message.id, message.dataLength, message.data);
    _tpHandler760->Process(_carState->CurrenTime);
}

void DiagnosticsContainer::GenerateMessages(MessageDirection direction)
{
    // No periodic messages to generate in diagnostics container.
}

void DiagnosticsContainer::HandleFeedbackSignal(FeedbackSignal signal)
{
    // React to signals and send immediate messages via the transport layer.
    switch (signal)
    {
        case FeedbackSignal::VinReadComplete:
        {
            if (_carState->SAVE_CONFIG)
            {
                _configFile->Write();
                _carState->SAVE_CONFIG = false;
            }
            break;
        }
        default:
            break;
    }
}

bool DiagnosticsContainer::CanAcceptMessage(const BusMessage& message)
{
    // No messages are accepted for forwarding in diagnostics container.
    return false;
}

void DiagnosticsContainer::HandleForwardedMessage(const BusMessage& message)
{
    // No forwarded messages to handle in diagnostics container.
}

void DiagnosticsContainer::UpdateMessages(uint64_t currentTime)
{
    // No periodic messages to update in diagnostics container.
}

void DiagnosticsContainer::ProcessImmediateSignal(ImmediateSignal signal)
{
    switch (signal)
    {
        case ImmediateSignal::StartVinRead:
        {
            //printf("DiagnosticsContainer::Starting VIN read via TpMessageHandler_760\n");
            _tpHandler760->SetRadioType(_carState->RADIO_TYPE);
            _tpHandler760->EnterDiagMode();
            _tpHandler760->Process(_carState->CurrenTime);
            break;
        }

        default:
            break;
    }
}

bool DiagnosticsContainer::CanParseMessage(const BusMessage& message)
{
    switch (message.id)
    {
        // Reply from diagnostic handler 0x760 - for reading VIN from radio
        case 0x660:
            return true;

        default:
            return false;
    }
}

void DiagnosticsContainer::SendImmediateMessage(uint32_t id)
{
    // No immediate messages to send in diagnostics container.
}

