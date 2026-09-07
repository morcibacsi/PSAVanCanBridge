#pragma once

#include "Platform/IDelayProvider.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class EspDelayProvider final : public IDelayProvider
{
public:
    void DelayMilliseconds(uint32_t milliseconds) override
    {
        vTaskDelay(pdMS_TO_TICKS(milliseconds));
    }
};
