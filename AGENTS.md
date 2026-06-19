"Please read AGENTS.md to understand our architectural constraints, directory layout, and real-time safety requirements before writing any code." It will dramatically increase the quality of the generated code generation.

## Scope

This project is ESP-IDF C++ firmware for ESP32-based automotive protocol bridging. It translates between PSA VAN/CAN protocol generations so newer ECUs or head units can be retrofitted into cars using older protocols.

When modifying code, preserve real-time behavior, bus timing, message periodicity, and hardware-specific board configuration.

## Source Layout

- `src/main.cpp`: application bootstrap, global service wiring, protocol selection, FreeRTOS task creation.
- `src/BoardConfig*.h`: board pinouts, transceiver options, supported protocol combinations.
- `src/Protocol`: protocol-neutral interfaces, transport layers, diagnostics, serial/websocket protocol adapters.
- `src/Protocol/AEE2001`: VAN comfort bus protocol support.
- `src/Protocol/AEE2004`: CAN AEE2004 comfort bus protocol support.
- `src/Protocol/AEE2010`: CAN AEE2010 comfort bus protocol support.
- `src/Protocol/*/Handlers`: message-specific parse/generate handlers grouped by ECU/domain.
- `src/Protocol/*/Structs`: packed frame/bitfield layouts for bus messages.
- `src/Helpers`: shared vehicle state, config, filesystem, time, CRC, web server, diagnostics helpers.
- `src/lib`: low-level CAN/VAN sender and receiver implementations.

## Core Architecture

`main.cpp` creates a source protocol handler and a destination protocol handler based on `CarState::SOURCE_PROTOCOL` and `CarState::DESTINATION_PROTOCOL`.

The main data flow is:

1. Transport layer receives a raw bus message.
2. Protocol handler tags it with a `ProtocolType`.
3. If the opposite protocol can directly accept the message, it forwards it.
4. Otherwise the matching message handler parses it into `CarState`.
5. Destination/source handlers generate outgoing messages from `CarState`.
6. `MessageScheduler` sends periodic messages through the active transport.

`CarState` is the shared translation model. Most protocol handlers should update or read `CarState` rather than directly coupling to another protocol generation.

## Important Interfaces

- `ITransportLayer`: common send/receive/logging interface for VAN, CAN, serial, and websocket-backed transports.
- `IProtocolHandler`: common protocol generation interface. Handles receive, parse, generate, forwarding, immediate signals, and scheduled updates.
- `IMessageHandler<Derived>`: CRTP-style base for message handlers.
- `BusMessage`: common message container. Contains id, command, data bytes, data length, CRC, protocol, message type, periodicity, offset, active flag, retry count, and priority.
- `MessageScheduler`: stores periodic messages by ID and sends them when due.

## Message Handler Pattern

Handlers usually live under:

`src/Protocol/<Generation>/Handlers/<ECU>/MessageHandler_<ID>_<Generation>.hpp`

A handler should generally provide:

- `static constexpr uint32_t MessageId`
- a private `BusMessage message` template for generation
- `BusMessage Generate(CarState* carState)`
- `void Parse(CarState* carState, const BusMessage& message)`

Example structure:
```cpp
class MessageHandler_2B6_AEE2004 : public IMessageHandler<MessageHandler_2B6_AEE2004>
{
    private:
        BusMessage message
        {
            .id = 0x2B6,
            .data = { 0 },
            .dataLength = 8,
            .protocol = ProtocolType::AEE2004,
            .periodicityMs = 1000-150,
            .offsetMs = 330,
            .isActive = true
        };
    public:
        static constexpr uint32_t MessageId = 0x2B6;

        BusMessage Generate(CarState* carState)
        {
            message.data[0] = carState->VIN_FOR_HEADUNIT[9];
            message.data[1] = carState->VIN_FOR_HEADUNIT[10];
            message.data[2] = carState->VIN_FOR_HEADUNIT[11];

            return message;
        }

        void Parse(CarState* carState, const BusMessage& msg)
        {
        }
};
```

Protocol classes keep handlers in a `std::tuple`. Supported message IDs are extracted at compile time with `ExtractMessageIds`.

When adding a new message handler:

1. Add the frame struct under the matching `Structs` folder if needed.
2. Add the handler under the appropriate generation/domain folder.
3. Include the handler in the corresponding `AEE20xxComfortBus.hpp`.
4. Add it to the handler tuple.
5. Register callbacks in `RegisterMessageHandlers` if the handler emits `ImmediateSignal` or `FeedbackSignal`.
6. Confirm `GenerateMessages`, `CanAcceptMessage`, `HandleForwardedMessage`, and `ProcessImmediateSignal` behavior if the message should be generated, forwarded, or sent immediately.

## Transport Notes

- `CANTransportLayer` sends synchronously through `ICanMessageSender`.
- `VANTransportLayer` queues TX messages and sends them from a dedicated FreeRTOS task.
- VAN TX checks `IsBusAvailable()` and retries before dropping a message.
- CAN message payloads are normally 8 bytes; VAN messages may use larger buffers.
- Be careful with `BusMessage::dataLength`; never copy more bytes than `BusMessage::data` can hold.

## Timing And Real-Time Constraints

- Main read/send loops run as FreeRTOS tasks.
- Message periodicity and offsets are encoded in each handler's generated `BusMessage`.
- `MessageScheduler::Update` is called frequently from send tasks.
- Avoid blocking operations in parse/generate paths.
- Avoid heap allocation in high-frequency paths unless consistent with existing code.
- Preserve `IRAM_ATTR` annotations where present.

## Signals

Use `ImmediateSignal` for events that should cause the opposite protocol to send messages immediately.

Use `FeedbackSignal` for events emitted by handlers that need local protocol-side follow-up, such as querying data, saving config, clock updates, or trip reset behavior.

## Configuration

Persistent user/device settings are loaded through `ConfigFile` from LittleFS into `CarState`.

Do not hardcode behavior that should be represented by existing `CarState` config flags. Prefer adding a clearly named config/state field when the behavior must be user-configurable.

## Board Support

Board variants are selected through `BoardConfig.h` and `PIO_INI_BOARD_*` macros.

When changing pin usage, CAN sender selection, ULP/HULP VAN TX, or second-CAN behavior, check all relevant `BoardConfig_*.h` files.

## Style Guidelines

- Follow existing C++ style: headers with `#pragma once`, small message handler classes, explicit includes.
- Keep protocol generation differences localized in the matching `AEE20xx` folder.
- Prefer structured frame definitions in `Structs` over ad hoc bit manipulation.
- Keep handler logic focused: parse raw message fields into `CarState`, or generate raw fields from `CarState`.
- Do not introduce broad refactors while adding or fixing one message.
- Be cautious with shared globals in `main.cpp`; they are part of the firmware wiring model.

## Safety Checklist Before Changes

- Does the change affect source protocol, destination protocol, or both?
- Does the message need parsing, generation, forwarding, or immediate sending?
- Is the message periodicity correct for the target protocol?
- Are callbacks registered if the handler emits signals?
- Is the correct transport used for the selected board?
- Are data lengths and struct sizes safe?
- Could this block a FreeRTOS task or disturb bus timing?
- Does the change preserve existing config behavior?