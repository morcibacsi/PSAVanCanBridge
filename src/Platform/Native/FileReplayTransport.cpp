#include "FileReplayTransport.hpp"

#include <cstdio>
#include <iomanip>
#include <limits>
#include <sstream>

namespace
{
    bool ParseTimestamp(const std::string& token, uint64_t& timestampMs)
    {
        std::string value = token;
        if (value.size() >= 2 && value.front() == '[' && value.back() == ']')
        {
            value = value.substr(1, value.size() - 2);
        }
        if (value.find(':') == std::string::npos)
        {
            std::istringstream stream(value);
            return static_cast<bool>(stream >> timestampMs);
        }

        unsigned hour = 0, minute = 0, second = 0, millisecond = 0;
        char separator1 = 0, separator2 = 0, decimal = 0;
        std::istringstream stream(value);
        if (!(stream >> hour >> separator1 >> minute >> separator2 >> second)
            || separator1 != ':' || separator2 != ':')
        {
            return false;
        }
        if (stream >> decimal)
        {
            if (decimal != '.' || !(stream >> millisecond))
            {
                return false;
            }
        }
        timestampMs = ((hour * 60ULL + minute) * 60ULL + second) * 1000ULL + millisecond;
        return true;
    }
}

FileReplayTransport::FileReplayTransport(
    std::string name,
    ProtocolType protocol,
    IApplicationClock* clock)
    : _name(std::move(name)), _protocol(protocol), _clock(clock)
{
}

bool FileReplayTransport::LoadCapture(const std::string& path, std::string& error)
{
    std::ifstream input(path);
    if (!input)
    {
        error = "Cannot open input capture: " + path;
        return false;
    }

    std::string line;
    uint64_t firstAbsoluteTime = 0;
    uint64_t previousAbsoluteTime = 0;
    uint64_t dayOffset = 0;
    bool haveFirstTime = false;
    size_t lineNumber = 0;
    while (std::getline(input, line))
    {
        ++lineNumber;
        if (line.empty() || line[0] == '#')
        {
            continue;
        }

        std::istringstream stream(line);
        std::string timestampToken;
        std::string idToken;
        if (!(stream >> timestampToken >> idToken))
        {
            continue;
        }

        uint64_t absoluteTime = 0;
        if (!ParseTimestamp(timestampToken, absoluteTime))
        {
            error = "Invalid timestamp on capture line " + std::to_string(lineNumber);
            return false;
        }
        if (haveFirstTime && absoluteTime + dayOffset < previousAbsoluteTime)
        {
            dayOffset += 24ULL * 60ULL * 60ULL * 1000ULL;
        }
        absoluteTime += dayOffset;
        if (!haveFirstTime)
        {
            firstAbsoluteTime = absoluteTime;
            haveFirstTime = true;
        }
        previousAbsoluteTime = absoluteTime;

        ReplayFrame frame{};
        frame.timestampMs = absoluteTime - firstAbsoluteTime;
        frame.message.protocol = _protocol;
        frame.message.type = MessageType::Normal;
        try
        {
            frame.message.id = static_cast<uint32_t>(std::stoul(idToken, nullptr, 16));
            std::string byteToken;
            while (stream >> byteToken)
            {
                if (frame.message.dataLength >= sizeof(frame.message.data))
                {
                    error = "Payload exceeds BusMessage capacity on capture line " + std::to_string(lineNumber);
                    return false;
                }
                frame.message.data[frame.message.dataLength++] =
                    static_cast<uint8_t>(std::stoul(byteToken, nullptr, 16));
            }
        }
        catch (...)
        {
            error = "Invalid hexadecimal value on capture line " + std::to_string(lineNumber);
            return false;
        }
        _replayFrames.push_back(frame);
    }

    if (_replayFrames.empty())
    {
        error = "Input capture contains no frames";
        return false;
    }
    return true;
}

bool FileReplayTransport::OpenOutput(const std::string& path, std::string& error)
{
    _output.open(path, std::ios::out | std::ios::trunc);
    if (!_output)
    {
        error = "Cannot open output capture: " + path;
        return false;
    }
    return true;
}

uint64_t FileReplayTransport::NextInputTimeMs() const
{
    return _replayFrames.empty() ? std::numeric_limits<uint64_t>::max() : _replayFrames.front().timestampMs;
}

uint8_t FileReplayTransport::SendMessage(const BusMessage& message, bool)
{
    CapturedTransmission transmission{_clock->NowMs(), message};
    _transmitted.push_back(transmission);
    WriteTransmission(transmission);
    if (_logger != nullptr)
    {
        _logger(_network, 2, message);
    }
    return 1;
}

bool FileReplayTransport::ReceiveMessage(BusMessage& message)
{
    if (_replayFrames.empty() || _replayFrames.front().timestampMs > _clock->NowMs())
    {
        return false;
    }
    message = _replayFrames.front().message;
    _replayFrames.pop_front();
    return true;
}

void FileReplayTransport::SetLoggerFunction(
    uint8_t network,
    void (*loggerFunction)(uint8_t, uint8_t, const BusMessage&))
{
    _network = network;
    _logger = loggerFunction;
}

void FileReplayTransport::WriteTransmission(const CapturedTransmission& transmission)
{
    std::ostringstream line;
    line << '[' << transmission.timestampMs << "] "
         << static_cast<unsigned>(transmission.message.protocol) << " TX "
         << std::uppercase << std::hex << std::setw(3) << std::setfill('0')
         << transmission.message.id;
    for (size_t i = 0; i < transmission.message.dataLength && i < sizeof(transmission.message.data); ++i)
    {
        line << ' ' << std::setw(2) << static_cast<unsigned>(transmission.message.data[i]);
    }
    std::printf("%s\n", line.str().c_str());
    if (_output)
    {
        _output << line.str() << '\n';
    }
}


