#pragma once

#include <cstdint>

#include "CarRadioRemote.h"

class RadioRemoteVolumeControl
{
public:
    enum class Pulse : uint8_t
    {
        None,
        VolumeMinus,
        VolumePlus
    };

    struct Result
    {
        CarRadioRemoteStruct remote{};
        Pulse pulse = Pulse::None;
    };

private:
    static constexpr uint64_t VolumeButtonWindowMs = 3000;
    static constexpr uint64_t ScrollContinuationWindowMs = 3000;

    struct ScrollState
    {
        uint8_t position = 0;
        bool overflowNegative = false;
        bool overflowPositive = false;

        static ScrollState From(const CarRadioRemoteStruct& remote)
        {
            return ScrollState
            {
                remote.data.scroll_position,
                remote.data.owerflow_scan_negative != 0,
                remote.data.owerflow_scan_positive != 0
            };
        }

        void ApplyTo(CarRadioRemoteStruct& remote) const
        {
            remote.data.scroll_position = position;
            remote.data.owerflow_scan_negative = overflowNegative;
            remote.data.owerflow_scan_positive = overflowPositive;
        }

        void Increment()
        {
            if (position == 0xFF)
            {
                position = 0x80;
                overflowPositive = !overflowPositive;
            }
            else
            {
                ++position;
            }
        }

        void Decrement()
        {
            if (position == 0x00)
            {
                position = 0x80;
                overflowNegative = !overflowNegative;
            }
            else
            {
                --position;
            }
        }

        void Advance(int16_t delta)
        {
            while (delta > 0)
            {
                Increment();
                --delta;
            }
            while (delta < 0)
            {
                Decrement();
                ++delta;
            }
        }
    };

    bool _initialized = false;
    bool _previousVolumeMinus = false;
    bool _previousVolumePlus = false;
    ScrollState _physicalScroll{};
    ScrollState _forwardedScroll{};
    uint64_t _volumeButtonWindowEnd = 0;
    uint64_t _scrollContinuationWindowEnd = 0;

    static bool IsWindowOpen(uint64_t currentTime, uint64_t windowEnd)
    {
        return windowEnd != 0 && currentTime <= windowEnd;
    }

    static int16_t ScrollDelta(const ScrollState& previous, const ScrollState& current)
    {
        if (current.overflowNegative != previous.overflowNegative)
        {
            return -static_cast<int16_t>(
                previous.position + 1 +
                static_cast<uint16_t>(0x80 - current.position));
        }
        if (current.overflowPositive != previous.overflowPositive)
        {
            return static_cast<int16_t>(
                static_cast<uint16_t>(0xFF - previous.position) + 1 +
                static_cast<uint16_t>(current.position - 0x80));
        }
        return static_cast<int16_t>(current.position) -
            static_cast<int16_t>(previous.position);
    }

public:
    Result Process(const CarRadioRemoteStruct& input, uint64_t currentTime)
    {
        Result result{input, Pulse::None};
        const ScrollState currentPhysicalScroll = ScrollState::From(input);

        if (!_initialized)
        {
            _initialized = true;
            _physicalScroll = currentPhysicalScroll;
            _forwardedScroll = currentPhysicalScroll;
        }

        const bool volumeMinusPressed = input.data.volume_minus != 0;
        const bool volumePlusPressed = input.data.volume_plus != 0;
        const bool volumeMinusPush = volumeMinusPressed && !_previousVolumeMinus;
        const bool volumePlusPush = volumePlusPressed && !_previousVolumePlus;

        if (volumeMinusPush || volumePlusPush)
        {
            _volumeButtonWindowEnd = currentTime + VolumeButtonWindowMs;
            _scrollContinuationWindowEnd = 0;
        }

        _previousVolumeMinus = volumeMinusPressed;
        _previousVolumePlus = volumePlusPressed;

        const int16_t scrollDelta = ScrollDelta(_physicalScroll, currentPhysicalScroll);
        _physicalScroll = currentPhysicalScroll;

        const bool volumeWindowOpen =
            IsWindowOpen(currentTime, _volumeButtonWindowEnd) ||
            IsWindowOpen(currentTime, _scrollContinuationWindowEnd);

        if (scrollDelta != 0 && volumeWindowOpen)
        {
            _scrollContinuationWindowEnd = currentTime + ScrollContinuationWindowMs;
            _forwardedScroll.ApplyTo(result.remote);
            result.pulse = scrollDelta > 0 ? Pulse::VolumePlus : Pulse::VolumeMinus;
            return result;
        }

        _forwardedScroll.Advance(scrollDelta);
        _forwardedScroll.ApplyTo(result.remote);
        return result;
    }
};
