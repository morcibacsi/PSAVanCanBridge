#pragma once

#include <cstdint>
#include <deque>
#include <fstream>
#include <string>
#include <vector>

#include "Application/IApplicationClock.hpp"
#include "Protocol/ITransportLayer.hpp"

class FileReplayTransport final : public ITransportLayer
{
public:
    struct CapturedTransmission
    {
        uint64_t timestampMs;
        BusMessage message;
    };

private:
    struct ReplayFrame
    {
        uint64_t timestampMs;
        BusMessage message;
    };

    std::string _name;
    ProtocolType _protocol;
    IApplicationClock* _clock;
    std::deque<ReplayFrame> _replayFrames;
    std::vector<CapturedTransmission> _transmitted;
    std::ofstream _output;
    uint8_t _network = 0;
    void (*_logger)(uint8_t, uint8_t, const BusMessage&) = nullptr;

    void WriteTransmission(const CapturedTransmission& transmission);

public:
    FileReplayTransport(std::string name, ProtocolType protocol, IApplicationClock* clock);

    bool LoadCapture(const std::string& path, std::string& error);
    bool OpenOutput(const std::string& path, std::string& error);
    bool HasPendingInput() const { return !_replayFrames.empty(); }
    uint64_t NextInputTimeMs() const;

    std::string Name() override { return _name; }
    uint8_t SendMessage(const BusMessage& message, bool highPriority = false) override;
    bool ReceiveMessage(BusMessage& message) override;
    bool IsBusAvailable() override { return true; }
    void SetLoggerFunction(
        uint8_t network,
        void (*loggerFunction)(uint8_t, uint8_t, const BusMessage&)) override;

    const std::vector<CapturedTransmission>& Transmitted() const { return _transmitted; }
};


