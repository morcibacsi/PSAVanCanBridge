#pragma once

#ifndef _CpuConfig_h
    #define _CpuConfig_h

#include <stdint.h>

typedef struct {
    uint8_t cpu_core;
    uint8_t priority;
    uint32_t stack_size;
} cpu_config_t;

#endif