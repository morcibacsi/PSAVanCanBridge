#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include "CarState.hpp"

class FuelRefillTracker
{
private:
    static constexpr const char* StateFileName = "/littlefs/fuel_tracker.json";
    static constexpr const char* TemporaryStateFileName = "/littlefs/fuel_tracker.tmp";
    static constexpr std::size_t HistoryCapacity = 5;
    static constexpr std::size_t TimestampSize = 24;
    static constexpr uint8_t SampleCount = 5;
    static constexpr uint64_t SampleIntervalMs = 2500;
    static constexpr uint64_t InputTimeoutMs = 5000;
    static constexpr uint8_t MaximumStableSpreadPercent = 2;
    static constexpr uint8_t RefillIncreasePercent = 8;
    static constexpr uint8_t CheckpointDecreasePercent = 2;

    struct Checkpoint
    {
        char timestamp[TimestampSize]{};
        uint32_t odometerRaw = 0;
        uint8_t fuelPercent = 0;
        bool valid = false;
    };

    struct Refill
    {
        char timestamp[TimestampSize]{};
        uint32_t odometerRaw = 0;
        uint8_t beforePercent = 0;
        uint8_t afterPercent = 0;
    };

    struct PersistedState
    {
        Checkpoint checkpoint{};
        Checkpoint checkpoints[HistoryCapacity]{};
        std::size_t checkpointCount = 0;
        Refill refills[HistoryCapacity]{};
        std::size_t refillCount = 0;
    };

    CarState* _carState;
    PersistedState _state{};
    Checkpoint& _checkpoint = _state.checkpoint;
    uint8_t _samples[SampleCount]{};
    uint8_t _sampleIndex = 0;
    uint8_t _sampleSize = 0;
    uint64_t _lastSampleTime = 0;
    uint64_t _lastFuelUpdateTime = 0;
    bool _startupCheckComplete = false;
    bool _previousIgnition = false;
    bool _ignitionStateInitialized = false;
    bool _offCheckpointHandled = false;

    bool IsWallClockValid() const;
    bool IsFuelLevelValid() const;
    void ResetSamples();
    void AddSample(uint8_t fuelPercent);
    bool GetStableFuelLevel(uint8_t& fuelPercent) const;
    bool LoadState();
    bool SaveState(const PersistedState& state) const;
    bool SaveCheckpoint(uint8_t fuelPercent, uint32_t odometerRaw);
    bool AppendRefill(uint8_t beforePercent, uint8_t afterPercent, uint32_t odometerRaw);
    bool IsLastRecordDuplicate(uint8_t beforePercent, uint8_t afterPercent, uint32_t odometerRaw) const;
    bool FormatTimestamp(char* buffer, std::size_t bufferSize) const;
    void ProcessStartup(uint8_t stableFuelPercent);
    void ProcessIgnitionOff(uint8_t stableFuelPercent);

public:
    enum class PersistenceFileResult
    {
        Success,
        NotFound,
        Error
    };
    explicit FuelRefillTracker(CarState* carState);
    void Process(uint64_t currentTime);
    PersistenceFileResult GetPersistedData(std::string& data) const;
    PersistenceFileResult DeletePersistedData() const;
};
