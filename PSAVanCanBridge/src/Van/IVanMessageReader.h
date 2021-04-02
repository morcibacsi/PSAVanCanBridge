#pragma once

#ifndef _IVanMessageReader_h
    #define _IVanMessageReader_h

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

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
    virtual void Init(uint8_t rxPin, uint8_t ledPin, IVAN_LINE_LEVEL vanLineLevel, IVAN_NETWORK_TYPE vanNetworkType) = 0;
    virtual void Stop() = 0;
    virtual bool IsCrcOk(uint8_t vanMessage[], uint8_t vanMessageLength) = 0;
};

#endif
