#include "FuelRefillTracker.hpp"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "cJSON.h"

namespace
{
constexpr std::size_t MaximumStateFileSize = 4096;

bool ReadUInt32(const cJSON* object, const char* name, uint32_t& value)
{
    const cJSON* item = cJSON_GetObjectItemCaseSensitive(object, name);
    if (!cJSON_IsNumber(item) || item->valuedouble < 0 || item->valuedouble > UINT32_MAX
        || item->valuedouble != static_cast<double>(static_cast<uint32_t>(item->valuedouble)))
    {
        return false;
    }

    value = static_cast<uint32_t>(item->valuedouble);
    return true;
}

bool ReadPercent(const cJSON* object, const char* name, uint8_t& value)
{
    uint32_t parsed = 0;
    if (!ReadUInt32(object, name, parsed) || parsed > 100)
    {
        return false;
    }

    value = static_cast<uint8_t>(parsed);
    return true;
}

template <typename Record>
bool ReadTimestamp(const cJSON* object, Record& record)
{
    const cJSON* timestamp = cJSON_GetObjectItemCaseSensitive(object, "timestamp");
    if (!cJSON_IsString(timestamp) || timestamp->valuestring == nullptr
        || timestamp->valuestring[0] == '\0'
        || std::strlen(timestamp->valuestring) >= sizeof(record.timestamp))
    {
        return false;
    }

    std::strcpy(record.timestamp, timestamp->valuestring);
    return true;
}
}

FuelRefillTracker::FuelRefillTracker(CarState* carState)
    : _carState(carState)
{
    LoadState();
}

FuelRefillTracker::PersistenceFileResult FuelRefillTracker::GetPersistedData(std::string& data) const
{
    data.clear();

    FILE* file = std::fopen(StateFileName, "r");
    if (file == nullptr)
    {
        return errno == ENOENT ? PersistenceFileResult::NotFound : PersistenceFileResult::Error;
    }

    char buffer[512];
    while (true)
    {
        std::size_t bytesRead = std::fread(buffer, 1, sizeof(buffer), file);
        if (bytesRead > 0)
        {
            if (data.size() + bytesRead > MaximumStateFileSize)
            {
                std::fclose(file);
                data.clear();
                return PersistenceFileResult::Error;
            }
            data.append(buffer, bytesRead);
        }

        if (bytesRead < sizeof(buffer))
        {
            bool success = !std::ferror(file) && std::fclose(file) == 0;
            if (!success)
            {
                data.clear();
                return PersistenceFileResult::Error;
            }
            return PersistenceFileResult::Success;
        }
    }
}

FuelRefillTracker::PersistenceFileResult FuelRefillTracker::DeletePersistedData() const
{
    if (std::remove(StateFileName) == 0)
    {
        return PersistenceFileResult::Success;
    }
    return errno == ENOENT ? PersistenceFileResult::NotFound : PersistenceFileResult::Error;
}

bool FuelRefillTracker::IsWallClockValid() const
{
    return _carState->Year >= 2020
        && _carState->Year <= 2099
        && _carState->Month >= 1
        && _carState->Month <= 12
        && _carState->MDay >= 1
        && _carState->MDay <= 31
        && _carState->Hour <= 23
        && _carState->Minute <= 59
        && _carState->Second <= 59;
}

bool FuelRefillTracker::IsFuelLevelValid() const
{
    return _carState->FuelLevel <= 100;
}

void FuelRefillTracker::ResetSamples()
{
    _sampleIndex = 0;
    _sampleSize = 0;
}

void FuelRefillTracker::AddSample(uint8_t fuelPercent)
{
    if (_carState->Ignition == 0)
    {
        return;
    }

    _samples[_sampleIndex] = fuelPercent;
    _sampleIndex = (_sampleIndex + 1) % SampleCount;
    if (_sampleSize < SampleCount)
    {
        _sampleSize++;
    }
}

bool FuelRefillTracker::GetStableFuelLevel(uint8_t& fuelPercent) const
{
    if (_sampleSize < SampleCount)
    {
        return false;
    }

    uint8_t sortedSamples[SampleCount];
    std::memcpy(sortedSamples, _samples, sizeof(sortedSamples));
    std::sort(sortedSamples, sortedSamples + SampleCount);

    if (sortedSamples[SampleCount - 1] - sortedSamples[0] > MaximumStableSpreadPercent)
    {
        return false;
    }

    fuelPercent = sortedSamples[SampleCount / 2];
    return true;
}

bool FuelRefillTracker::FormatTimestamp(char* buffer, std::size_t bufferSize) const
{
    if (!IsWallClockValid())
    {
        return false;
    }

    int written = std::snprintf(
        buffer,
        bufferSize,
        "%04u-%02u-%02uT%02u:%02u:%02u",
        static_cast<unsigned>(_carState->Year),
        static_cast<unsigned>(_carState->Month),
        static_cast<unsigned>(_carState->MDay),
        static_cast<unsigned>(_carState->Hour),
        static_cast<unsigned>(_carState->Minute),
        static_cast<unsigned>(_carState->Second));

    return written > 0 && static_cast<std::size_t>(written) < bufferSize;
}

bool FuelRefillTracker::LoadState()
{
    FILE* file = std::fopen(StateFileName, "r");
    if (file == nullptr)
    {
        return false;
    }

    if (std::fseek(file, 0, SEEK_END) != 0)
    {
        std::fclose(file);
        return false;
    }

    long fileSize = std::ftell(file);
    if (fileSize <= 0 || static_cast<std::size_t>(fileSize) > MaximumStateFileSize
        || std::fseek(file, 0, SEEK_SET) != 0)
    {
        std::fclose(file);
        printf("Fuel tracker state is empty or invalid\n");
        return false;
    }

    char* buffer = static_cast<char*>(std::malloc(static_cast<std::size_t>(fileSize) + 1));
    if (buffer == nullptr)
    {
        std::fclose(file);
        printf("Failed to allocate fuel tracker state buffer\n");
        return false;
    }

    std::size_t bytesRead = std::fread(buffer, 1, static_cast<std::size_t>(fileSize), file);
    int closeResult = std::fclose(file);
    buffer[bytesRead] = '\0';
    if (bytesRead != static_cast<std::size_t>(fileSize) || closeResult != 0)
    {
        std::free(buffer);
        printf("Failed to read fuel tracker state\n");
        return false;
    }

    cJSON* root = cJSON_Parse(buffer);
    std::free(buffer);
    if (!cJSON_IsObject(root))
    {
        cJSON_Delete(root);
        printf("Fuel tracker state contains malformed JSON\n");
        return false;
    }

    PersistedState loaded{};
    auto parseCheckpoint = [](const cJSON* object, Checkpoint& checkpoint) {
        return cJSON_IsObject(object)
            && ReadTimestamp(object, checkpoint)
            && ReadUInt32(object, "odometer", checkpoint.odometerRaw)
            && ReadPercent(object, "fuelPercent", checkpoint.fuelPercent);
    };
    auto parseRefill = [](const cJSON* object, Refill& refill) {
        return cJSON_IsObject(object)
            && ReadTimestamp(object, refill)
            && ReadUInt32(object, "odometer", refill.odometerRaw)
            && ReadPercent(object, "beforePercent", refill.beforePercent)
            && ReadPercent(object, "afterPercent", refill.afterPercent);
    };

    const cJSON* checkpoint = cJSON_GetObjectItemCaseSensitive(root, "checkpoint");
    loaded.checkpoint.valid = parseCheckpoint(checkpoint, loaded.checkpoint);

    const cJSON* checkpoints = cJSON_GetObjectItemCaseSensitive(root, "checkpoints");
    if (cJSON_IsArray(checkpoints))
    {
        for (int index = 0; index < cJSON_GetArraySize(checkpoints); ++index)
        {
            Checkpoint parsed{};
            if (parseCheckpoint(cJSON_GetArrayItem(checkpoints, index), parsed))
            {
                if (loaded.checkpointCount == HistoryCapacity)
                {
                    std::move(loaded.checkpoints + 1, loaded.checkpoints + HistoryCapacity, loaded.checkpoints);
                    --loaded.checkpointCount;
                }
                parsed.valid = true;
                loaded.checkpoints[loaded.checkpointCount++] = parsed;
            }
        }
    }

    const cJSON* refills = cJSON_GetObjectItemCaseSensitive(root, "refills");
    if (cJSON_IsArray(refills))
    {
        for (int index = 0; index < cJSON_GetArraySize(refills); ++index)
        {
            Refill parsed{};
            if (parseRefill(cJSON_GetArrayItem(refills, index), parsed))
            {
                if (loaded.refillCount == HistoryCapacity)
                {
                    std::move(loaded.refills + 1, loaded.refills + HistoryCapacity, loaded.refills);
                    --loaded.refillCount;
                }
                loaded.refills[loaded.refillCount++] = parsed;
            }
        }
    }

    cJSON_Delete(root);
    _state = loaded;
    return loaded.checkpoint.valid || loaded.checkpointCount != 0 || loaded.refillCount != 0;
}

bool FuelRefillTracker::SaveState(const PersistedState& state) const
{
    cJSON* root = cJSON_CreateObject();
    cJSON* checkpoints = cJSON_CreateArray();
    cJSON* refills = cJSON_CreateArray();
    if (root == nullptr || checkpoints == nullptr || refills == nullptr)
    {
        cJSON_Delete(root);
        cJSON_Delete(checkpoints);
        cJSON_Delete(refills);
        return false;
    }

    auto createCheckpoint = [](const Checkpoint& checkpoint) {
        cJSON* item = cJSON_CreateObject();
        if (item != nullptr)
        {
            cJSON_AddStringToObject(item, "timestamp", checkpoint.timestamp);
            cJSON_AddNumberToObject(item, "odometer", checkpoint.odometerRaw);
            cJSON_AddNumberToObject(item, "fuelPercent", checkpoint.fuelPercent);
        }
        return item;
    };
    auto createRefill = [](const Refill& refill) {
        cJSON* item = cJSON_CreateObject();
        if (item != nullptr)
        {
            cJSON_AddStringToObject(item, "timestamp", refill.timestamp);
            cJSON_AddNumberToObject(item, "odometer", refill.odometerRaw);
            cJSON_AddNumberToObject(item, "beforePercent", refill.beforePercent);
            cJSON_AddNumberToObject(item, "afterPercent", refill.afterPercent);
        }
        return item;
    };

    bool valid = !state.checkpoint.valid
        || cJSON_AddItemToObject(root, "checkpoint", createCheckpoint(state.checkpoint));
    if (!state.checkpoint.valid)
    {
        valid = cJSON_AddNullToObject(root, "checkpoint") != nullptr;
    }
    valid = valid && cJSON_AddItemToObject(root, "refills", refills)
        && cJSON_AddItemToObject(root, "checkpoints", checkpoints);
    if (!valid)
    {
        cJSON_Delete(root);
        return false;
    }

    for (std::size_t index = 0; index < state.refillCount; ++index)
    {
        cJSON* item = createRefill(state.refills[index]);
        if (item == nullptr || !cJSON_AddItemToArray(refills, item))
        {
            cJSON_Delete(item);
            cJSON_Delete(root);
            return false;
        }
    }
    for (std::size_t index = 0; index < state.checkpointCount; ++index)
    {
        cJSON* item = createCheckpoint(state.checkpoints[index]);
        if (item == nullptr || !cJSON_AddItemToArray(checkpoints, item))
        {
            cJSON_Delete(item);
            cJSON_Delete(root);
            return false;
        }
    }

    char* json = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    if (json == nullptr)
    {
        return false;
    }

    FILE* file = std::fopen(TemporaryStateFileName, "w");
    bool success = file != nullptr;
    if (success)
    {
        bool writeSuccess = std::fputs(json, file) >= 0;
        success = std::fclose(file) == 0 && writeSuccess;
    }
    std::free(json);

    if (!success)
    {
        std::remove(TemporaryStateFileName);
        printf("Failed to write fuel tracker state\n");
        return false;
    }

    if (std::rename(TemporaryStateFileName, StateFileName) != 0)
    {
        std::remove(TemporaryStateFileName);
        printf("Failed to replace fuel tracker state\n");
        return false;
    }
    return true;
}

bool FuelRefillTracker::SaveCheckpoint(uint8_t fuelPercent, uint32_t odometerRaw)
{
    printf(
        "Saving fuel refill checkpoint: %u%% at odometer %lu\n",
        static_cast<unsigned>(fuelPercent),
        static_cast<unsigned long>(odometerRaw));

    Checkpoint checkpoint{};
    if (!FormatTimestamp(checkpoint.timestamp, sizeof(checkpoint.timestamp)))
    {
        return false;
    }
    checkpoint.odometerRaw = odometerRaw;
    checkpoint.fuelPercent = fuelPercent;
    checkpoint.valid = true;

    PersistedState updated = _state;
    updated.checkpoint = checkpoint;
    bool duplicate = updated.checkpointCount != 0
        && updated.checkpoints[updated.checkpointCount - 1].odometerRaw == odometerRaw
        && updated.checkpoints[updated.checkpointCount - 1].fuelPercent == fuelPercent;
    if (!duplicate)
    {
        if (updated.checkpointCount == HistoryCapacity)
        {
            std::move(updated.checkpoints + 1, updated.checkpoints + HistoryCapacity, updated.checkpoints);
            --updated.checkpointCount;
        }
        updated.checkpoints[updated.checkpointCount++] = checkpoint;
    }

    if (!SaveState(updated))
    {
        return false;
    }
    _state = updated;
    return true;
}

bool FuelRefillTracker::IsLastRecordDuplicate(
    uint8_t beforePercent,
    uint8_t afterPercent,
    uint32_t odometerRaw) const
{
    if (_state.refillCount == 0)
    {
        return false;
    }

    const Refill& last = _state.refills[_state.refillCount - 1];

    uint32_t odometerDifference = last.odometerRaw > odometerRaw
        ? last.odometerRaw - odometerRaw
        : odometerRaw - last.odometerRaw;

    return last.beforePercent == beforePercent
        && last.afterPercent == afterPercent
        && odometerDifference <= 10;
}

bool FuelRefillTracker::AppendRefill(
    uint8_t beforePercent,
    uint8_t afterPercent,
    uint32_t odometerRaw)
{
    printf(
        "Appending fuel refill record: %u%% -> %u%% at odometer %lu\n",
        static_cast<unsigned>(beforePercent),
        static_cast<unsigned>(afterPercent),
        static_cast<unsigned long>(odometerRaw));

    if (IsLastRecordDuplicate(beforePercent, afterPercent, odometerRaw))
    {
        printf("Duplicate fuel refill record suppressed\n");
        return true;
    }

    Refill refill{};
    if (!FormatTimestamp(refill.timestamp, sizeof(refill.timestamp)))
    {
        return false;
    }
    refill.odometerRaw = odometerRaw;
    refill.beforePercent = beforePercent;
    refill.afterPercent = afterPercent;

    PersistedState updated = _state;
    if (updated.refillCount == HistoryCapacity)
    {
        std::move(updated.refills + 1, updated.refills + HistoryCapacity, updated.refills);
        --updated.refillCount;
    }
    updated.refills[updated.refillCount++] = refill;
    if (!SaveState(updated))
    {
        return false;
    }
    _state = updated;

    printf(
        "Fuel refill detected: %u%% -> %u%% at odometer %lu\n",
        static_cast<unsigned>(beforePercent),
        static_cast<unsigned>(afterPercent),
        static_cast<unsigned long>(odometerRaw));
    return true;
}

void FuelRefillTracker::ProcessStartup(uint8_t stableFuelPercent)
{
    printf("ProcessStartup: stableFuelPercent = %u%%\n", static_cast<unsigned>(stableFuelPercent));
    uint32_t odometerRaw = _carState->Odometer.asUint24;

    if (!_checkpoint.valid)
    {
        if (SaveCheckpoint(stableFuelPercent, odometerRaw))
        {
            _startupCheckComplete = true;
        }
        return;
    }

    if (stableFuelPercent >= _checkpoint.fuelPercent + RefillIncreasePercent)
    {
        if (AppendRefill(_checkpoint.fuelPercent, stableFuelPercent, odometerRaw)
            && SaveCheckpoint(stableFuelPercent, odometerRaw))
        {
            _startupCheckComplete = true;
        }
        return;
    }

    if (_checkpoint.fuelPercent >= stableFuelPercent + CheckpointDecreasePercent)
    {
        if (!SaveCheckpoint(stableFuelPercent, odometerRaw))
        {
            return;
        }
    }

    _startupCheckComplete = true;
}

void FuelRefillTracker::ProcessIgnitionOff(uint8_t stableFuelPercent)
{
    printf("ProcessIgnitionOff: stableFuelPercent = %u%%\n", static_cast<unsigned>(stableFuelPercent));

    if (!_checkpoint.valid)
    {
        return;
    }

    if (_checkpoint.fuelPercent >= stableFuelPercent + CheckpointDecreasePercent)
    {
        SaveCheckpoint(stableFuelPercent, _carState->Odometer.asUint24);
    }
}

void FuelRefillTracker::Process(uint64_t currentTime)
{
    if (!_carState->ENABLE_FUEL_REFILL_TRACKING)
    {
        return;
    }

    bool inputsAreFresh = currentTime <= InputTimeoutMs;

/*
    _carState->FuelLevelLastUpdateTime != 0
        && _carState->OdometerLastUpdateTime != 0
        && currentTime - _carState->FuelLevelLastUpdateTime <= InputTimeoutMs
        && currentTime - _carState->OdometerLastUpdateTime <= InputTimeoutMs;
        printf(
            "FuelRefillTracker::Process: currentTime = %llu, inputsAreFresh = %s\n",
            currentTime,
            inputsAreFresh ? "true" : "false");
*/
    if (!IsWallClockValid() || !IsFuelLevelValid() || inputsAreFresh)
    {
        ResetSamples();
        return;
    }

    bool ignition = _carState->Ignition != 0;
    if (!_ignitionStateInitialized)
    {
        _previousIgnition = ignition;
        _ignitionStateInitialized = true;
    }
    else if (ignition != _previousIgnition)
    {
        _previousIgnition = ignition;
        _offCheckpointHandled = false;
        if (ignition)
        {
            _startupCheckComplete = false;
        }
        ResetSamples();
        _lastSampleTime = 0;
    }

    if (_lastSampleTime != 0 && currentTime - _lastSampleTime < SampleIntervalMs)
    {
        //printf("Waiting for next sample interval: %llu ms remaining\n", SampleIntervalMs - (currentTime - _lastSampleTime));
        return;
    }
/*
    if (_lastFuelUpdateTime == _carState->FuelLevelLastUpdateTime)
    {
        return;
    }
*/
    printf(
        "Processing fuel refill tracker: currentTime = %llu, lastSampleTime = %llu, sampleInterval = %llu\n",
        currentTime,
        _lastSampleTime,
        SampleIntervalMs);
    _lastSampleTime = currentTime;
    //_lastFuelUpdateTime = _carState->FuelLevelLastUpdateTime;
    AddSample(_carState->FuelLevel);

    uint8_t stableFuelPercent = 0;
    if (!GetStableFuelLevel(stableFuelPercent))
    {
        return;
    }

    if (ignition && !_startupCheckComplete)
    {
        ProcessStartup(stableFuelPercent);
        return;
    }

    if (!ignition && !_offCheckpointHandled)
    {
        ProcessIgnitionOff(stableFuelPercent);
        _offCheckpointHandled = true;
    }
}