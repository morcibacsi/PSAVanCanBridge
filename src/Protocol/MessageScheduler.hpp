#pragma once

#include <cstdint>
#include <unordered_map>
#include <cstring>

#include "BusMessage.hpp"
#include "ITransportLayer.hpp"
#include "../Platform/PlatformMutex.hpp"

struct MessageMetadata {
    BusMessage message;       // The actual bus message.
    uint32_t periodicityMs;   // Periodicity in milliseconds.
    uint64_t nextDueTime;     // Timestamp of the next generation/send slot.
    uint64_t lastSentTime;    // Timestamp of the last send (e.g., milliseconds since epoch).
};

class MessageScheduler {
private:
    std::unordered_map<uint32_t, MessageMetadata> scheduledMessages;
    PlatformRecursiveMutex mutex;

public:
    MessageScheduler() = default;

    bool HasScheduledMessages()
    {
        bool hasMessages = false;
        if (mutex.Lock(10))
        {
            hasMessages = !scheduledMessages.empty();
            mutex.Unlock();
        }
        return hasMessages;
    }

    bool HasDueMessages(uint64_t currentTime)
    {
        bool hasDueMessages = false;
        if (mutex.Lock(10))
        {
            for (const auto& [id, scheduled] : scheduledMessages)
            {
                (void)id;
                if (currentTime >= scheduled.nextDueTime)
                {
                    hasDueMessages = true;
                    break;
                }
            }
            mutex.Unlock();
        }
        return hasDueMessages;
    }

    bool ShouldGenerateMessage(uint32_t id, uint64_t currentTime)
    {
        bool shouldGenerate = true;
        if (mutex.Lock(10))
        {
            auto it = scheduledMessages.find(id);
            if (it != scheduledMessages.end())
            {
                shouldGenerate = currentTime >= it->second.nextDueTime;
            }
            mutex.Unlock();
        }
        return shouldGenerate;
    }

    void AddOrUpdateMessage(const BusMessage& message, uint64_t currentTime)
    {
        if (mutex.Lock(10))
        {
            auto it = scheduledMessages.find(message.id);
            if (it != scheduledMessages.end())
            {
                // Update the existing message
                it->second.message = message;

                size_t copyLen = message.dataLength;
                if (message.dataLength > sizeof(it->second.message.data))
                {
                    copyLen = sizeof(it->second.message.data);
                }

                std::memcpy(it->second.message.data, message.data, copyLen);
                it->second.message.dataLength = copyLen;
                it->second.periodicityMs = message.periodicityMs;
            }
            else
            {
                // Add a new scheduled message
                MessageMetadata newMessage{
                    message,
                    message.periodicityMs,
                    (uint64_t)currentTime + message.offsetMs,
                    0
                };
                scheduledMessages[message.id] = newMessage;
            }
            mutex.Unlock();
        }
    }

    void Update(uint64_t currentTime, ITransportLayer& transportLayer)
    {
        //printf("Update messages start\n");

        if (mutex.Lock(10))
        {
            for (auto& [id, scheduled] : scheduledMessages)
            {
                /*
                printf("Message ID: %03X | Current Time: %llu | Next Due At: %llu | Periodicity: %u\n",
                    id, currentTime, scheduled.nextDueTime, scheduled.periodicityMs);
                */
                //printf("Scheduled message ID: %03X\n", id);
                if (currentTime >= scheduled.nextDueTime)
                {
                    if (scheduled.message.isActive)
                    {
                        //printf("%s: %03X\n", transportLayer.Name().c_str(), (unsigned int) scheduled.message.id);
                        transportLayer.SendMessage(scheduled.message);
                        scheduled.lastSentTime = currentTime;
                    }

                    if (scheduled.periodicityMs > 0)
                    {
                        const uint64_t elapsedPeriods =
                            (currentTime - scheduled.nextDueTime) / scheduled.periodicityMs;
                        scheduled.nextDueTime += (elapsedPeriods + 1) * scheduled.periodicityMs;
                    }
                    else
                    {
                        scheduled.nextDueTime = currentTime;
                    }
                }
            }
            mutex.Unlock();
        }
        //printf("Update messages end\n");
    }

    void SendImmedateMessage(uint16_t id, uint64_t currentTime, ITransportLayer& transportLayer)
    {
        if (mutex.Lock(10))
        {
            auto it = scheduledMessages.find(id);
            if (it != scheduledMessages.end())
            {
                it->second.lastSentTime = currentTime;

                transportLayer.SendMessage(it->second.message);
            }
            mutex.Unlock();
        }
    }
};
