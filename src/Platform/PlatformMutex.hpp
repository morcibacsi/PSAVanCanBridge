#pragma once

#include <cstdint>

#ifdef PLATFORM_NATIVE
#include <chrono>
#include <mutex>

class PlatformRecursiveMutex
{
    std::recursive_timed_mutex _mutex;

public:
    bool Lock(uint32_t timeoutMs)
    {
        return _mutex.try_lock_for(std::chrono::milliseconds(timeoutMs));
    }

    void Unlock()
    {
        _mutex.unlock();
    }
};

#else
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

class PlatformRecursiveMutex
{
    SemaphoreHandle_t _mutex;

public:
    PlatformRecursiveMutex() : _mutex(xSemaphoreCreateRecursiveMutex()) {}

    bool Lock(uint32_t timeoutMs)
    {
        return xSemaphoreTakeRecursive(_mutex, pdMS_TO_TICKS(timeoutMs)) == pdTRUE;
    }

    void Unlock()
    {
        xSemaphoreGiveRecursive(_mutex);
    }
};
#endif

