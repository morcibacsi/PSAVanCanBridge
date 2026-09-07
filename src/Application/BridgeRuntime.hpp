#pragma once

#include "IApplicationClock.hpp"
#include "Helpers/CarState.hpp"
#include "Protocol/IProtocolHandler.hpp"

class BridgeRuntime
{
public:
    using PeriodicHook = void (*)(void* context, uint64_t currentTime);
    using DiagnosticHook = void (*)(void* context, uint64_t currentTime, const BusMessage& message);
    using MessageHook = void (*)(void* context, const BusMessage& message);

    struct Hooks
    {
        void* context = nullptr;
        PeriodicHook periodic = nullptr;
        DiagnosticHook diagnostic = nullptr;
        MessageHook sourceMessage = nullptr;
    };

private:
    CarState* _carState;
    IApplicationClock* _clock;
    IProtocolHandler* _source;
    IProtocolHandler* _destination;
    IProtocolHandler* _diagnostics;
    Hooks _hooks;

    static BridgeRuntime* _instance;
    static void ImmediateSignalTrampoline(ImmediateSignal signal);
    static void EmptyImmediateSignal(ImmediateSignal) {}

public:
    BridgeRuntime(
        CarState* carState,
        IApplicationClock* clock,
        IProtocolHandler* source,
        IProtocolHandler* destination,
        IProtocolHandler* diagnostics,
        Hooks hooks);

    bool Initialize();
    bool ProcessSourceOnce();
    bool ProcessDestinationOnce();
    void ProcessDestinationOutputOnce();
    void ProcessSourceOutputOnce();
    void ProcessOnce();
};
