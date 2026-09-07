#include "BridgeRuntime.hpp"

BridgeRuntime* BridgeRuntime::_instance = nullptr;

BridgeRuntime::BridgeRuntime(
    CarState* carState,
    IApplicationClock* clock,
    IProtocolHandler* source,
    IProtocolHandler* destination,
    IProtocolHandler* diagnostics,
    Hooks hooks)
    : _carState(carState),
      _clock(clock),
      _source(source),
      _destination(destination),
      _diagnostics(diagnostics),
      _hooks(hooks)
{
}

bool BridgeRuntime::Initialize()
{
    if (_carState == nullptr || _clock == nullptr || _source == nullptr || _destination == nullptr)
    {
        return false;
    }

    _instance = this;
    _destination->RegisterMessageHandlers(EmptyImmediateSignal);
    _source->RegisterMessageHandlers(ImmediateSignalTrampoline);
    if (_diagnostics != nullptr)
    {
        _diagnostics->RegisterMessageHandlers(ImmediateSignalTrampoline);
    }
    return true;
}

void BridgeRuntime::ImmediateSignalTrampoline(ImmediateSignal signal)
{
    if (_instance == nullptr)
    {
        return;
    }
    _instance->_destination->ProcessImmediateSignal(signal);
    if (_instance->_diagnostics != nullptr)
    {
        _instance->_diagnostics->ProcessImmediateSignal(signal);
    }
}

bool BridgeRuntime::ProcessSourceOnce()
{
    BusMessage message{};
    if (!_source->ReceiveMessage(message))
    {
        return false;
    }
    if (message.id == 0)
    {
        return true;
    }
    if (_hooks.sourceMessage != nullptr)
    {
        _hooks.sourceMessage(_hooks.context, message);
    }
    if (_destination->CanAcceptMessage(message))
    {
        _destination->HandleForwardedMessage(message);
    }
    else if (_source->CanParseMessage(message))
    {
        _source->ParseMessage(message);
    }
    return true;
}

bool BridgeRuntime::ProcessDestinationOnce()
{
    BusMessage message{};
    if (!_destination->ReceiveMessage(message))
    {
        return false;
    }
    if (message.id == 0)
    {
        return true;
    }
    if (_source->CanAcceptMessage(message))
    {
        _source->HandleForwardedMessage(message);
        return true;
    }
    if (_destination->CanParseMessage(message))
    {
        _destination->ParseMessage(message);
    }
    if (!_carState->DiagConnected && _diagnostics != nullptr && _diagnostics->CanParseMessage(message))
    {
        _diagnostics->ParseMessage(message);
    }
    else if (_carState->DiagConnected && _hooks.diagnostic != nullptr)
    {
        _hooks.diagnostic(_hooks.context, _clock->NowMs(), message);
    }
    return true;
}

void BridgeRuntime::ProcessDestinationOutputOnce()
{
    _carState->CurrenTime = _clock->NowMs();
    if (_hooks.periodic != nullptr)
    {
        _hooks.periodic(_hooks.context, _carState->CurrenTime);
    }
    _destination->GenerateMessages(IProtocolHandler::MessageDirection::Destination);
    _destination->UpdateMessages(_carState->CurrenTime);
}

void BridgeRuntime::ProcessSourceOutputOnce()
{
    _source->GenerateMessages(IProtocolHandler::MessageDirection::Source);
    _source->UpdateMessages(_carState->CurrenTime);
}

void BridgeRuntime::ProcessOnce()
{
    _carState->CurrenTime = _clock->NowMs();
    while (ProcessSourceOnce()) {}
    while (ProcessDestinationOnce()) {}
    ProcessDestinationOutputOnce();
    ProcessSourceOutputOnce();
}
