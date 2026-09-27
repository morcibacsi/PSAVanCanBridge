#pragma once

#include <utility>

#include "IVanMessageReceiver.h"
#include "IVanMessageSender.h"

// Application-side adapter for a VAN backend whose public API matches the
// bridge interfaces but which deliberately does not depend on them.
template<typename Backend>
class VanMessageSenderReceiverAdapter : public IVanMessageSender, public IVanMessageReceiver
{
private:
    Backend _backend;

protected:
    Backend& BackendInstance() { return _backend; }
    const Backend& BackendInstance() const { return _backend; }

public:
    template<typename... Args>
    explicit VanMessageSenderReceiverAdapter(Args&&... args)
        : _backend(std::forward<Args>(args)...)
    {
    }

    void Start() override
    {
        _backend.Start();
    }

    void SendNormalFrame(const uint16_t identifier, const uint8_t data[],
                         const uint8_t length, const bool requireAck) override
    {
        _backend.SendNormalFrame(identifier, data, length, requireAck);
    }

    void SendReplyRequestFrame(const uint16_t identifier) override
    {
        _backend.SendReplyRequestFrame(identifier);
    }

    bool IsTxPossible() override
    {
        return _backend.IsTxPossible();
    }

    void SetAckIdentifiers(const uint16_t identifiers[], const uint8_t count) override
    {
        (void)_backend.SetAckIdentifiers(identifiers, count);
    }

    void SetAckIdentifier(const uint8_t slot, const uint16_t identifier,
                          const bool enabled) override
    {
        (void)_backend.SetAckIdentifier(slot, identifier, enabled);
    }

    void SetQueryRequesterAckEnabled(const bool enabled) override
    {
        (void)_backend.SetQueryRequesterAckEnabled(enabled);
    }

    void SetRequestedReplyFrame(const uint8_t slot, const uint16_t identifier,
                                const uint8_t data[], const uint8_t length,
                                const bool enabled) override
    {
        (void)_backend.SetRequestedReplyFrame(slot, identifier, data, length, enabled);
    }

    void ReceiveData(uint8_t* messageLength, uint8_t message[]) override
    {
        _backend.ReceiveData(messageLength, message);
    }
};
