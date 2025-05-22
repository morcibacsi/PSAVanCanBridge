#pragma once

#include <cstdint>
#include <unordered_map>
#include <cstring>

#include "BusMessage.hpp"
#include "ITransportLayer.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

struct MessageMetadata {
    BusMessage message;       // The actual bus message.
    uint32_t periodicityMs;   // Periodicity in milliseconds.
    uint64_t lastSentTime;    // Timestamp of the last send (e.g., milliseconds since epoch).
};

class MessageScheduler {
private:
    std::unordered_map<uint32_t, MessageMetadata> scheduledMessages;
    SemaphoreHandle_t mutex;

public:
    MessageScheduler()
    {
        mutex = xSemaphoreCreateRecursiveMutex();
    }

    void AddOrUpdateMessage(const BusMessage& message, uint64_t currentTime)
    {
        if (xSemaphoreTakeRecursive(mutex, pdMS_TO_TICKS(10)))
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
                MessageMetadata newMessage{message, message.periodicityMs, (uint64_t)currentTime + message.offsetMs};
                scheduledMessages[message.id] = newMessage;
            }
            xSemaphoreGiveRecursive(mutex);
        }
    }

    void Update(uint64_t currentTime, ITransportLayer& transportLayer)
    {
        //printf("Update messages start\n");

        if (xSemaphoreTakeRecursive(mutex, pdMS_TO_TICKS(10)))
        {
            for (auto& [id, scheduled] : scheduledMessages)
            {
                /*
                printf("Message ID: %03X | Current Time: %llu | Next Send At: %llu | Periodicity: %u\n",
                    id, currentTime, scheduled.lastSentTime, scheduled.periodicityMs);
                */
                //printf("Scheduled message ID: %03X\n", id);
                if (scheduled.message.isActive && (currentTime - scheduled.lastSentTime) >= scheduled.periodicityMs)
                {
                    //printf("%s: %03X\n", transportLayer.Name().c_str(), (unsigned int) scheduled.message.id);
                    transportLayer.SendMessage(scheduled.message);
                    scheduled.lastSentTime = currentTime;
                }
            }
            xSemaphoreGiveRecursive(mutex);
        }
        //printf("Update messages end\n");
    }

    void SendImmedateMessage(uint16_t id, uint64_t currentTime, ITransportLayer& transportLayer)
    {
        if (xSemaphoreTakeRecursive(mutex, pdMS_TO_TICKS(10)))
        {
            auto it = scheduledMessages.find(id);
            if (it != scheduledMessages.end())
            {
                it->second.lastSentTime = currentTime;

                transportLayer.SendMessage(it->second.message);
            }
            xSemaphoreGiveRecursive(mutex);
        }
    }
};