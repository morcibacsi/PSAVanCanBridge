#pragma once

#ifndef _IVanMessageReader_h
    #define _IVanMessageReader_h

#include <stdint.h>

typedef enum {
    LINE_LEVEL_LOW = 0,
    LINE_LEVEL_HIGH = 1,
} IVAN_LINE_LEVEL;

typedef enum {
    NETWORK_TYPE_BODY = 0,
    NETWORK_TYPE_COMFORT = 1,
} IVAN_NETWORK_TYPE;

class IVanMessageReader {
public:
    virtual void Receive(uint8_t* messageLength, uint8_t message[]) = 0;
    virtual void Init() = 0;
    virtual void Stop() = 0;
    virtual bool IsCrcOk(uint8_t vanMessage[], uint8_t vanMessageLength) = 0;
};

#endif
